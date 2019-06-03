#include "COpenGLDriver.h"

#include "COpenGLHelper.h"
#include "COpenGLMaterialRenderComponent.h"
#include "COpenGLTextureWriteComponent.h"
#include "COpenGLShaderManageComponent.h"
#include "COpenGLDrawHelperComponent.h"
#include "COpenGLRenderTarget.h"
#include "COpenGLVertexDeclaration.h"
#include "COpenGLVertexIndexBuffer.h"
#include "IVertexIndexBuffer.h"
#include "CFileSystem.h"
#include "stringext.h"
#include "function3d.h"

COpenGLDriver::COpenGLDriver()
	: IVideoDriver(EDT_OPENGL)
{
	HWnd = nullptr;
	Hdc = nullptr;
	Hgrc = nullptr;

	ColorFormat = ECF_A8R8G8B8;
	DepthFormat = ECF_D24S8;

	DefaultFrameBuffer = 0;

	//
	MaterialRenderComponent = std::make_unique<COpenGLMaterialRenderComponent>(this);
	TextureWriteComponent = std::make_unique<COpenGLTextureWriteComponent>(this);
	ShaderManageComponent = std::make_unique<COpenGLShaderManageComponent>(this);
	DrawHelperComponent = std::make_unique<COpenGLDrawHelperComponent>(this);
}

COpenGLDriver::~COpenGLDriver()
{
	FrameBufferRT.reset();

	for (int i = 0; i < EVT_COUNT; ++i)
	{
		VertexDeclarations[i].reset();
	}

	//
	DrawHelperComponent.reset();
	ShaderManageComponent.reset();
	TextureWriteComponent.reset();
	MaterialRenderComponent.reset();

	if (!wglMakeCurrent(nullptr, nullptr))
	{
		ASSERT(false);
	}

	if (Hgrc && !wglDeleteContext(Hgrc))
	{
		ASSERT(false);
	}

	if (Hdc && HWnd)
		::ReleaseDC(HWnd, Hdc);
}

bool COpenGLDriver::initDriver(const SWindowInfo& wndInfo, bool vsync, E_AA_MODE aaMode)
{
	ASSERT(::IsWindow(wndInfo.hwnd));

	HWnd = wndInfo.hwnd;

	DriverSetting.vsync = vsync;
	DriverSetting.aaMode = aaMode;

	dimension2d windowSize = getWindowSize();

	Hdc = ::GetDC(HWnd);

	const uint8_t depthBits = 24;
	const uint8_t stencilBits = 8;

	// Set up pixel format descriptor with desired parameters
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),             // Size Of This Pixel Format Descriptor
		1,                                         // Version Number
		PFD_DRAW_TO_WINDOW |                       // Format Must Support Window
		PFD_SUPPORT_OPENGL |                       // Format Must Support OpenGL
		PFD_DOUBLEBUFFER | // Must Support Double Buffering
						   //	PFD_STEREO,        // Must Support Stereo Buffer
		PFD_TYPE_RGBA,                             // Request An RGBA Format
		32,                               // Select Our Color Depth
		0, 0, 0, 0, 0, 0,                          // Color Bits Ignored
		0,                                         // No Alpha Buffer
		0,                                         // Shift Bit Ignored
		0,                                         // No Accumulation Buffer
		0, 0, 0, 0,	                               // Accumulation Bits Ignored
		depthBits,                        // Z-Buffer (Depth Buffer)
		stencilBits,              // Stencil Buffer Depth
		0,                                         // No Auxiliary Buffer
		PFD_MAIN_PLANE,                            // Main Drawing Layer
		0,                                         // Reserved
		0, 0, 0                                    // Layer Masks Ignored
	};

	//choose pixel format
	int pixelFormat;

	//depth stencil
	pixelFormat = ChoosePixelFormat(Hdc, &pfd);
	DepthFormat = ECF_D24S8;
	if (!pixelFormat)
	{
		pfd.cDepthBits = 15;
		pfd.cStencilBits = 1;

		pixelFormat = ChoosePixelFormat(Hdc, &pfd);
		DepthFormat = ECF_D16;
	}

	if (!pixelFormat)
	{
		ASSERT(false);
		return false;
	}

	if (!SetPixelFormat(Hdc, pixelFormat, &pfd))
	{
		ASSERT(false);
		return false;
	}

	Hgrc = wglCreateContext(Hdc);
	if (!Hgrc)
	{
		::ReleaseDC(HWnd, Hdc);

		ASSERT(false);
		return false;
	}

	if (!wglMakeCurrent(Hdc, Hgrc))
	{
		wglDeleteContext(Hgrc);
		::ReleaseDC(HWnd, Hdc);

		return false;
	}

	g_FileSystem->writeLog(ELOG_GX, "Render System: OpenGL");

	//device info
	AdapterInfo.index = 0;

	const char* str = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	AdapterInfo.description = str;
	const char* str1 = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	AdapterInfo.name = std_string_format("OpenGL %s", str1);
	const char* str2 = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	AdapterInfo.vendorName = str2;

	//log
	g_FileSystem->writeLog(ELOG_GX, "Adapter Name: %s", AdapterInfo.name.c_str());
	g_FileSystem->writeLog(ELOG_GX, "Adpater Description: %s", AdapterInfo.description.c_str());
	g_FileSystem->writeLog(ELOG_GX, "Adapter Vendor: %s", AdapterInfo.vendorName.c_str());

	if (!GLExtension.initExtensions())
	{
		g_FileSystem->writeLog(ELOG_GX, "Video Card Extension does not support some features. Application will now exit!");
		return false;
	}
	g_FileSystem->writeLog(ELOG_GX, "OpenGL driver Version is %0.2f", GLExtension.Version * 0.01f);
	g_FileSystem->writeLog(ELOG_GX, "OpenGL shader language Version is %d", GLExtension.ShaderLanguageVersion);

	//antialias
	uint8_t antialias = 0;
	switch (DriverSetting.aaMode)
	{
	case E_AA_MSAA_1:
		antialias = 1;
		break;
	case E_AA_MSAA_2:
		antialias = 2;
		break;
	case E_AA_MSAA_3:
		antialias = 3;
		break;
	case E_AA_MSAA_4:
		antialias = 4;
		break;
	default:
		antialias = 0;
		break;
	}

	if (antialias > 0)
	{
		if (antialias * 2 > GLExtension.MaxMultiSample)
			antialias = GLExtension.MaxMultiSample / 2;
	}
	DriverSetting.antialias = antialias;

	IsSupportA8L8 = GLExtension.TextureCompressionLA || GLExtension.TextureCompressionRG;
	IsMultiSampleEnabled = (DriverSetting.aaMode >= E_AA_MSAA_1 && DriverSetting.aaMode <= E_AA_MSAA_4) && GLExtension.MultisampleSupported;
	IsSupportDepthTexture = GLExtension.queryOpenGLFeature(IRR_ARB_depth_texture);

	g_FileSystem->writeLog(ELOG_GX,
		"IsSupportA8L8 : %d, IsMultiSampleEnabled : %d, IsFxAAEnabled: %d",
		IsSupportA8L8 ? 1 : 0,
		IsMultiSampleEnabled ? 1 : 0,
		isFXAAEnabled() ? 1 : 0);

	//vsync
#ifdef WGL_EXT_swap_control
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
	// vsync extension
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	// set vsync
	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(vsync ? 1 : 0);
#endif

	if (!queryFeature(EVDF_PIXEL_SHADER_3_0) ||		//ps_1_1 not allowed
		!queryFeature(EVDF_STREAM_OFFSET) ||
		!queryFeature(EVDF_RENDER_TO_TARGET) ||
		!queryFeature(EVDF_MIP_MAP) ||
		!queryFeature(EVDF_TEXTURE_ADDRESS) ||
		!queryFeature(EVDF_SEPARATE_UVWRAP) ||
		!GLExtension.TextureCompressionS3 ||
		!GLExtension.queryOpenGLFeature(IRR_EXT_packed_depth_stencil))
	{
		g_FileSystem->writeLog(ELOG_GX, "Video Card does not support some features. Application will now exit!");
		return false;
	}

	//backbuffer format
	int pf = GetPixelFormat(Hdc);
	DescribePixelFormat(Hdc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	if (pfd.cAlphaBits != 0)
	{
		if (pfd.cRedBits == 8)
			ColorFormat = ECF_A8R8G8B8;
		else if (pfd.cGreenBits == 5)
			ColorFormat = ECF_A1R5G5B5;
	}
	else
	{
		if (pfd.cRedBits == 8)
			ColorFormat = ECF_A8R8G8B8;
		else if (pfd.cGreenBits == 6)
			ColorFormat = ECF_R5G6B5;
		else if (pfd.cGreenBits == 5)
			ColorFormat = ECF_A1R5G5B5;
	}

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &DefaultFrameBuffer);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	ASSERT_OPENGL_SUCCESS();

	for (int i = 0; i < EVT_COUNT; ++i)
	{
		VertexDeclarations[i] = std::make_unique<COpenGLVertexDeclaration>(this, (E_VERTEX_TYPE)i);
	}

	if (!MaterialRenderComponent->init() ||
		!TextureWriteComponent->init() ||
		!ShaderManageComponent->init() ||
		!DrawHelperComponent->init())
	{
		ASSERT(false);
	}

	FrameBufferRT = std::make_unique<COpenGLRenderTarget>(this, windowSize, ColorFormat, DepthFormat, true);

	setDisplayMode(windowSize);

	return true;
}

bool COpenGLDriver::beginScene()
{
	PrimitivesDrawn = 0;
	DrawCall = 0;

	return true;
}

bool COpenGLDriver::endScene()
{
	if (GLExtension.DiscardFrameSupported)
	{
		if (CurrentRenderTarget != nullptr)
		{
			const COpenGLRenderTarget* rt = static_cast<const COpenGLRenderTarget*>(CurrentRenderTarget);
			GLsizei num = rt->getnumAttachments();
			GLExtension.extGlInvalidateFramebuffer(GL_FRAMEBUFFER, num, rt->getAttachments());
		}
	}

	return SwapBuffers(Hdc) != FALSE;
}

bool COpenGLDriver::clear(bool renderTarget, bool zBuffer, bool stencil, SColor color)
{
	GLbitfield mask = 0;

	if (renderTarget)
		mask |= GL_COLOR_BUFFER_BIT;

	if (zBuffer)
	{
		MaterialRenderComponent->setZWriteEnable(true);
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if (stencil)
		mask |= GL_STENCIL_BUFFER_BIT;

	if (mask)
	{
		MaterialRenderComponent->setClearColor(color);

		glClear(mask);
		ASSERT_OPENGL_SUCCESS();
	}

	return true;
}

bool COpenGLDriver::setRenderTarget(const IRenderTarget * texture, bool bindDepth)
{
	const COpenGLRenderTarget* tex = static_cast<const COpenGLRenderTarget*>(texture);

	if (tex == nullptr)
	{
		GLExtension.extGlBindFramebuffer(GL_FRAMEBUFFER, DefaultFrameBuffer);		//restore
		setViewPort(recti(0, 0, ScreenSize.width, ScreenSize.height));
	}
	else
	{
		tex->bindFrameBuffer(bindDepth);
		GLExtension.extGlBindFramebuffer(GL_FRAMEBUFFER, tex->getFrameBuffer());
		setViewPort(recti(0, 0, tex->getSize().width, tex->getSize().height));
	}

	bool success = GLExtension.checkFBOStatus();
	ASSERT(success);

	CurrentRenderTarget = texture;

	return true;
}

void COpenGLDriver::setTransform(E_TRANSFORMATION_STATE state, const matrix4& mat)
{
	ASSERT(0 <= state && state < ETS_COUNT);
	switch (state)
	{
	case ETS_VIEW:
	{
		V = mat;
		VP = V * P;
		T_V = V; T_V.transpose();
		T_VP = VP; T_VP.transpose();
	}
	break;
	case ETS_WORLD:
	{
		W = mat;
		T_W = W; T_W.transpose();
	}
	break;

	case ETS_PROJECTION:
	{
		P = mat;
		VP = V * P;
		T_P = P; T_P.transpose();
		T_VP = VP; T_VP.transpose(); 
	}
	break;
	default:
		break;
	}
}

void COpenGLDriver::setTexture(int index, ITexture * tex)
{
	MaterialRenderComponent->setCurrentTexture(index, tex);
}

dimension2d COpenGLDriver::getWindowSize() const
{
	RECT rc;
	::GetClientRect(HWnd, &rc);
	return dimension2d((uint32_t)rc.right - rc.left, (uint32_t)rc.bottom - rc.top);
}

void COpenGLDriver::logCaps()
{
	g_FileSystem->writeLog(ELOG_GX, "Device Caps:");
	if (queryFeature(EVDF_PIXEL_SHADER_3_0))
		g_FileSystem->writeLog(ELOG_GX, "\tPixel Shader: 3.0");
	else if (queryFeature(EVDF_PIXEL_SHADER_2_0))
		g_FileSystem->writeLog(ELOG_GX, "\tPixel Shader: 2.0");
	else
		g_FileSystem->writeLog(ELOG_GX, "\tPixel Shader 2.0 minimum not supported!");

	if (queryFeature(EVDF_VERTEX_SHADER_3_0))
		g_FileSystem->writeLog(ELOG_GX, "\tVertex Shader: 3.0");
	else if (queryFeature(EVDF_VERTEX_SHADER_2_0))
		g_FileSystem->writeLog(ELOG_GX, "\tVertex Shader: 2.0");
	else
		g_FileSystem->writeLog(ELOG_GX, "\tVertex Shader 2.0 minimum not supported!");

	if (GLExtension.TextureCompressionS3)
		g_FileSystem->writeLog(ELOG_GX, "\tTexture Compression s3tc supported.");

	if (GLExtension.TextureCompressionPVR)
		g_FileSystem->writeLog(ELOG_GX, "\tTexture Compression pvrtc supported.");

	if (GLExtension.TextureCompressionETC1)
		g_FileSystem->writeLog(ELOG_GX, "\tTexture Compression etc1 supported.");

	if (GLExtension.TextureCompressionATC)
		g_FileSystem->writeLog(ELOG_GX, "\tTexture Compression atc supported.");

	if (GLExtension.TextureCompression3DC)
		g_FileSystem->writeLog(ELOG_GX, "\tTexture Compression 3dc supported.");

	if (GLExtension.TextureCompressionLA)
		g_FileSystem->writeLog(ELOG_GX, "\tTexture Compression la supported.");

	if (GLExtension.TextureCompressionRG)
		g_FileSystem->writeLog(ELOG_GX, "\tTexture Compression rg supported.");

	if (!GLExtension.TextureCompression)
		g_FileSystem->writeLog(ELOG_GX, "\tTexture Compression not supported.");
}

bool COpenGLDriver::queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
{
	return GLExtension.queryFeature(feature);
}

bool COpenGLDriver::reset()
{
	MaterialRenderComponent->resetRSCache();

	FrameBufferRT = std::make_unique<COpenGLRenderTarget>(this, ScreenSize, ColorFormat, DepthFormat, true);

	g_FileSystem->writeLog(ELOG_GX, "reset... Device Format: %dX%d, %s", ScreenSize.width, ScreenSize.height, getColorFormatString(ColorFormat));

	return true;
}

void COpenGLDriver::setVertexDeclarationAndBuffers(const CGLProgram* program, const IVertexBuffer* vbuffer0, uint32_t offset0, const IIndexBuffer* ibuffer)
{
	COpenGLVertexDeclaration* decl = getVertexDeclaration(vbuffer0->getVertexType());
	ASSERT(decl);
	decl->apply(program, vbuffer0, offset0);

	if (ibuffer)
	{
		ASSERT(static_cast<const COpenGLIndexBuffer*>(ibuffer)->getHWBuffer());
		GLExtension.extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<const COpenGLIndexBuffer*>(ibuffer)->getHWBuffer());
	}
	else
	{
		GLExtension.extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void COpenGLDriver::drawIndexedPrimitive(const IVertexBuffer* vbuffer, const IIndexBuffer* ibuffer, const CGLProgram* program, E_PRIMITIVE_TYPE primType, uint32_t primCount, const SDrawParam & drawParam)
{
	if (!drawParam.numVertices || drawParam.numVertices >= 65536 || !primCount || drawParam.baseVertIndex)
	{
		ASSERT(false);
		return;
	}

	ASSERT(IVideoResource::hasVideoBuilt(vbuffer));
	ASSERT(!ibuffer || IVideoResource::hasVideoBuilt(ibuffer));

	GLenum mode = COpenGLHelper::getGLTopology(primType);
	GLenum type = 0;
	uint32_t indexSize = 0;
	if (ibuffer)
	{
		switch (ibuffer->getIndexType())
		{
		case EIT_16BIT:
		{
			type = GL_UNSIGNED_SHORT;
			indexSize = 2;
			break;
		}
		case EIT_32BIT:
		{
			type = GL_UNSIGNED_INT;
			indexSize = 4;
			break;
		}
		}

		setVertexDeclarationAndBuffers(program,
			vbuffer, drawParam.voffset0,
			ibuffer);

		//buffer check
		/*
		{
			int nIndices = getIndexCount(primType, primCount);
			ASSERT(ibuffer->getIndexType() == EIT_16BIT);
			int nVertices = (int)vbuffer->getNumVertices() - (int)drawParam.voffset0;
			ASSERT(nVertices > 0);
			{
				for (int i = drawParam.startIndex; i < (int)drawParam.startIndex + nIndices; ++i)
				{
					const uint16_t* pIndices = (const uint16_t*)ibuffer->getIndices();
					ASSERT(pIndices[i] < nVertices);
				}
			}
		}
		*/

		GLExtension.extGlDrawElements(mode,
			getIndexCount(primType, primCount),
			type,
			COpenGLHelper::buffer_offset(indexSize * drawParam.startIndex));
	}
	else
	{
		setVertexDeclarationAndBuffers(program,
			vbuffer, drawParam.voffset0,
			ibuffer);

		glDrawArrays(mode, drawParam.startIndex, getIndexCount(primType, primCount));
		ASSERT_OPENGL_SUCCESS();
	}

	if (primType == EPT_TRIANGLES || primType == EPT_TRIANGLE_STRIP)
	{
		PrimitivesDrawn += primCount;
	}

	++DrawCall;
}

void COpenGLDriver::setViewPort(const recti& area)
{
	recti vp = area;
	vp.clipAgainst(recti(0, 0, ScreenSize.width, ScreenSize.height));
	if (vp.getWidth() <= 0 || vp.getHeight() <= 0)
		return;

	MaterialRenderComponent->setViewport(vp);

	Viewport = vp;

	makeVPScaleMatrix(vp);

	float fWidth = vp.getWidth() * 0.5f;
	float fHeight = vp.getHeight() * 0.5f;

	View2DTM = f3d::makeViewMatrix(vector3df(fWidth + OrthoCenterOffset.x, fHeight + OrthoCenterOffset.y, 0),
		vector3df::UnitZ(), vector3df::UnitY(), 0.0f);
	Project2DTM = f3d::makeOrthoOffCetnerMatrixLH(-fWidth, fWidth, -fHeight, fHeight, 0.0f, 1.0f);

	VP2D = View2DTM * Project2DTM;
	T_VP2D = VP2D; T_VP2D.transpose();
}

void COpenGLDriver::setDisplayMode(const dimension2d& size)
{
	ScreenSize = size;

	reset();

	setViewPort(recti(0, 0, ScreenSize.width, ScreenSize.height));
}

bool COpenGLDriver::setDriverSetting(const SDriverSetting & setting)
{
	bool vsyncChanged = false;
	bool antialiasChanged = false;

	bool vsync = setting.vsync;
	uint8_t antialias = setting.antialias;

	if (vsync != DriverSetting.vsync)
	{
#ifdef WGL_EXT_swap_control
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
		// vsync extension
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		// set vsync
		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(vsync ? 1 : 0);
#endif

		vsyncChanged = true;

		DriverSetting.vsync = vsync;
	}

	if (antialias != DriverSetting.antialias)
	{
		antialiasChanged = true;

		DriverSetting.antialias = antialias;
	}

	bool ret = true;
	if (vsyncChanged || antialiasChanged)
	{
		// 		if (beginScene())
		// 		{
		// 			clear(true, false, false, SColor(0,0,0));
		// 			endScene();
		// 		}
		ret = reset();

		setViewPort(recti(0, 0, ScreenSize.width, ScreenSize.height));

		if (ret)
		{
			g_FileSystem->writeLog(ELOG_GX, "Driver Setting Changed. Vsync: %s, Antialias: %d, %s",
				DriverSetting.vsync ? "On" : "Off",
				(int32_t)DriverSetting.antialias,
				"Window");
		}
	}
	return ret;
}

ITextureWriter* COpenGLDriver::createTextureWriter(ITexture* texture)
{
	return TextureWriteComponent->createTextureWriter(texture);
}

bool COpenGLDriver::removeTextureWriter(ITexture * texture)
{
	return TextureWriteComponent->removeTextureWriter(texture);
}

void COpenGLDriver::draw(IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, E_PRIMITIVE_TYPE primType, uint32_t primCount, const SDrawParam& drawParam)
{
	IVideoResource::buildVideoResources(vbuffer);
	if (ibuffer)
		IVideoResource::buildVideoResources(ibuffer);

	const CGLProgram* program = ShaderManageComponent->applyShaders(Material, vbuffer->getVertexType());
	MaterialRenderComponent->setRenderStates(Material, GlobalMaterial, program);


	//TODO

	MaterialRenderComponent->applyRenderStates();
}

void COpenGLDriver::deleteVao(const IVertexBuffer* vbuffer)
{
	E_VERTEX_TYPE vType = vbuffer->getVertexType();
	if (vType != EVT_INVALID)
	{
		VertexDeclarations[vType]->deleteVao(vbuffer);
	}
}
