#include "COpenGLMaterialRenderComponent.h"

#include "IVideoResource.h"
#include "COpenGLHelper.h"
#include "COpenGLDriver.h"
#include "COpenGLTexture.h"
#include "COpenGLShaderManageComponent.h"
#include "function.h"

#define  DEVICE_SET_CLEARCOLOR(prop, v)	if (RsCache.prop != (v))		\
								{			\
			glClearColor(v.r, v.g, v.b, v.a); 	\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}

#define DEVICE_SET_VIEWPORT(prop, v) if (RsCache.prop != (v))		\
								{	\
		glViewport(v.left, v.top, v.getWidth(), v.getHeight()); 	\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_BOOL_STATE(prop, d3drs, v)	if (RsCache.prop != (v))		\
								{	v != GL_FALSE ? glEnable(d3drs) : glDisable(d3drs);				\
		ASSERT_OPENGL_SUCCESS();				\
		RsCache.prop = (v);	}

#define  DEVICE_SET_DEPTHFUNC_STATE(prop, v)	if (RsCache.prop != (v))		\
								{	glDepthFunc(v);				\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}

#define DEVICE_SET_COLORWRITEMASK_STATE(prop, v)	{	const GLboolean* bv = (v);	\
	if (memcmp(RsCache.prop, bv, sizeof(GLboolean) * 4) != 0)		\
			{	glColorMask(bv[0], bv[1], bv[2], bv[3]);		\
			ASSERT_OPENGL_SUCCESS();				\
			memcpy(RsCache.prop, bv, sizeof(GLboolean) * 4);		} } 

#define  DEVICE_SET_DEPTHMASK_STATE(prop, v)	if (RsCache.prop != (v))		\
								{	glDepthMask(v);				\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_CULLFACE_STATE(prop, v)	if (RsCache.prop != (v))		\
								{	glCullFace(v);				\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_FRONTFACE_STATE(prop, v)	if (RsCache.prop != (v))		\
								{	glFrontFace(v);				\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_LINEWIDTH(prop, v) if (RsCache.prop != (v))		\
								{	glLineWidth(v);				\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}

#define DEVICE_SET_BLEND_STATE(prop1, prop2, v1, v2) if (RsCache.prop1 != (v1) || RsCache.prop2 != (v2))		\
				{	glBlendFunc(v1, v2); 					\
	ASSERT_OPENGL_SUCCESS();		\
	RsCache.prop1 = v1; RsCache.prop2 = v2; }

#define DEVICE_SET_ALPHAFUNC_STATE(prop1, prop2, v1, v2) if (RsCache.prop1 != (v1) || RsCache.prop2 != (v2))		\
				{	glAlphaFunc(v1, v2); 					\
	ASSERT_OPENGL_SUCCESS();			\
	RsCache.prop1 = v1; RsCache.prop2 = v2; }

#define DEVICE_SET_TEXTURE2D_PARAMETER_I(st, prop, d3drs, v)		\
				{	glTexParameteri(GL_TEXTURE_2D, d3drs, v);				\
	ASSERT_OPENGL_SUCCESS();			\
	RsCache.TextureUnits[st].prop = (v);	}

#define DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, prop, d3drs, v)		\
				{	glTexParameteri(GL_TEXTURE_CUBE_MAP, d3drs, v);				\
	ASSERT_OPENGL_SUCCESS();			\
	RsCache.TextureUnits[st].prop = (v);	}


COpenGLMaterialRenderComponent::COpenGLMaterialRenderComponent(const COpenGLDriver* driver)
	: Driver(driver)
{

}

bool COpenGLMaterialRenderComponent::init()
{
	resetRSCache();

	CurrentRenderState = RsCache;

	return true;
}

void COpenGLMaterialRenderComponent::setRenderStates(const CPass* pass, const SGlobalMaterial* globalMaterial, const CGLProgram* program)
{
	// zbuffer
	{
		CurrentRenderState.ZEnable = pass->ZBuffer == ECFN_NEVER ? GL_FALSE : GL_TRUE;
		CurrentRenderState.ZFunc = COpenGLHelper::getGLCompare(pass->ZBuffer);
	}

	// zwrite
	{
		CurrentRenderState.ZWriteEnable = pass->ZWriteEnable ? GL_TRUE : GL_FALSE;
	}

	// backface culling
	{
		GLenum cullmode;
		GLenum frontface = GL_CW;
		switch (pass->Cull)
		{
		case ECM_FRONT:
			cullmode = GL_FRONT;
			break;
		case ECM_BACK:
			cullmode = GL_BACK;
			break;
		case ECM_NONE:
		default:
			cullmode = GL_FRONT_AND_BACK;
			break;
		}

		CurrentRenderState.FrontFace = frontface;
		CurrentRenderState.CullMode = cullmode;
		CurrentRenderState.CullEnable = pass->Cull != ECM_NONE ? GL_TRUE : GL_FALSE;
	}

	// anti aliasing
	if (Driver->getDriverSetting().antialias)
	{
		bool multisample = false;
		bool antialiasline = false;
		switch (pass->AntiAliasing)
		{
		case EAAM_SIMPLE:
			multisample = true;
			break;
		case EAAM_LINE_SMOOTH:
			multisample = true;
			antialiasline = true;
			break;
		case EAAM_OFF:
		default:
			break;
		}

		CurrentRenderState.MultiSampleAntiAlias = multisample ? GL_TRUE : GL_FALSE;
		CurrentRenderState.AntiAliasedLineEnable = antialiasline ? GL_TRUE : GL_FALSE;
	}
	
	//texture unit
	for (const auto& itr : pass->getMaterial()->TextureVariableMap)
	{
		int idx = program->getTextureIndex(itr.first.c_str());
		if (idx == -1)			//material中的texture变量未使用
		{
			CurrentRenderState.TextureUnits[idx].texture = nullptr;				//未使用的texture置空
			continue;
		}
		ASSERT(idx >= 0 && idx < MATERIAL_MAX_TEXTURES);

		const STextureUnit& texUnit = itr.second;
		CurrentRenderState.TextureUnits[idx].addressS = COpenGLHelper::getGLTextureAddress(texUnit.TextureWrapU);
		CurrentRenderState.TextureUnits[idx].addressT = COpenGLHelper::getGLTextureAddress(texUnit.TextureWrapV);

		CurrentRenderState.TextureUnits[idx].texture = texUnit.Texture;

		//global material
		{
			GLint tftMag, tftMin, tftMip;

			// Bilinear, trilinear, and anisotropic filter
			GLint maxAnisotropy = 1;
			if (globalMaterial->TextureFilter != ETF_NONE)
			{
				uint8_t anisotropic = getAnisotropic(globalMaterial->TextureFilter);
				tftMag = GL_LINEAR;
				tftMin = GL_LINEAR;
				tftMip = globalMaterial->TextureFilter > ETF_BILINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST;

				if (Driver->GLExtension.queryOpenGLFeature(IRR_EXT_texture_filter_anisotropic))
					maxAnisotropy = min_(anisotropic, Driver->GLExtension.MaxAnisotropy);
			}
			else
			{
				tftMag = GL_NEAREST;
				tftMin = GL_NEAREST;
				tftMip = GL_NEAREST_MIPMAP_NEAREST;
			}

			CurrentRenderState.TextureUnits[idx].magFilter = tftMag;
			CurrentRenderState.TextureUnits[idx].minFilter = tftMin;
			CurrentRenderState.TextureUnits[idx].mipFilter = tftMip;
			CurrentRenderState.TextureUnits[idx].maxAniso = maxAnisotropy;
		}
	}

	//blend state
	CurrentRenderState.AlphaBlendEnable = pass->AlphaBlendEnabled ? GL_TRUE : GL_FALSE;
	CurrentRenderState.SrcBlend = COpenGLHelper::getGLBlend(pass->SrcBlend);
	CurrentRenderState.DestBlend = COpenGLHelper::getGLBlend(pass->DestBlend);
}

void COpenGLMaterialRenderComponent::applyRenderStates()
{
	DEVICE_SET_BOOL_STATE(ZEnable, GL_DEPTH_TEST, CurrentRenderState.ZEnable);
	if (CurrentRenderState.ZEnable)
	{
		DEVICE_SET_DEPTHFUNC_STATE(ZFunc, CurrentRenderState.ZFunc);
	}

	DEVICE_SET_DEPTHMASK_STATE(ZWriteEnable, CurrentRenderState.ZWriteEnable);
	//DEVICE_SET_BOOL_STATE(StencilEnable, GL_STENCIL_TEST, CurrentRenderState.StencilEnable);
	//DEVICE_SET_BOOL_STATE(ScissorEnable, GL_SCISSOR_TEST, CurrentRenderState.ScissorEnable);
	DEVICE_SET_BOOL_STATE(CullEnable, GL_CULL_FACE, CurrentRenderState.CullEnable);

	if (CurrentRenderState.CullEnable)
	{
		DEVICE_SET_CULLFACE_STATE(CullMode, CurrentRenderState.CullMode);
	}
	DEVICE_SET_FRONTFACE_STATE(FrontFace, CurrentRenderState.FrontFace);
	if (Driver->GLExtension.queryOpenGLFeature(IRR_ARB_multisample))
	{
		DEVICE_SET_BOOL_STATE(MultiSampleAntiAlias, GL_MULTISAMPLE_ARB, CurrentRenderState.MultiSampleAntiAlias);
	}
	DEVICE_SET_BOOL_STATE(AntiAliasedLineEnable, GL_LINE_SMOOTH, CurrentRenderState.AntiAliasedLineEnable);

	DEVICE_SET_COLORWRITEMASK_STATE(ColorMask, CurrentRenderState.ColorMask);
	DEVICE_SET_BOOL_STATE(AlphaBlendEnable, GL_BLEND, CurrentRenderState.AlphaBlendEnable);
	if (CurrentRenderState.AlphaBlendEnable)
	{
		DEVICE_SET_BLEND_STATE(SrcBlend, DestBlend, CurrentRenderState.SrcBlend, CurrentRenderState.DestBlend);
	}

	//texture units
	for (uint32_t st = 0; st < MATERIAL_MAX_TEXTURES; ++st)
	{
		const SRenderStateCache::STextureUnit& texunit = CurrentRenderState.TextureUnits[st];

		setActiveTexture(st);

		if (!texunit.texture)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			ASSERT_OPENGL_SUCCESS();
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			ASSERT_OPENGL_SUCCESS();
			RsCache.TextureUnits[st].texture = nullptr;
			continue;
		}

		IVideoResource::buildVideoResources(texunit.texture);

		ASSERT(texunit.texture->getSampleCount() == 0);
		bool bCube = texunit.texture->isCube();

		const COpenGLTexture* gltex = static_cast<const COpenGLTexture*>(texunit.texture);
		glBindTexture(bCube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, gltex->getGLTexture());
		ASSERT_OPENGL_SUCCESS();
		RsCache.TextureUnits[st].texture = texunit.texture;

		if (!bCube)
		{
			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressS, GL_TEXTURE_WRAP_S, texunit.addressS);

			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressT, GL_TEXTURE_WRAP_T, texunit.addressT);

			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, magFilter, GL_TEXTURE_MAG_FILTER, texunit.magFilter);

			if (texunit.texture->getNumMipmaps() > 1)
			{
				DEVICE_SET_TEXTURE2D_PARAMETER_I(st, mipFilter, GL_TEXTURE_MIN_FILTER, texunit.mipFilter);
			}
			else
			{
				DEVICE_SET_TEXTURE2D_PARAMETER_I(st, minFilter, GL_TEXTURE_MIN_FILTER, texunit.minFilter);
			}
		}
		else
		{
			DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, addressS, GL_TEXTURE_WRAP_S, texunit.addressS);

			DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, addressT, GL_TEXTURE_WRAP_T, texunit.addressT);

			GLint tftMag, tftMin, tftMip;
			tftMag = GL_NEAREST;
			tftMin = GL_NEAREST;
			tftMip = GL_NEAREST_MIPMAP_NEAREST;

			DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, magFilter, GL_TEXTURE_MAG_FILTER, tftMag);

			if (texunit.texture->getNumMipmaps() > 1)
			{
				DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, mipFilter, GL_TEXTURE_MIN_FILTER, tftMip);
			}
			else
			{
				DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, minFilter, GL_TEXTURE_MIN_FILTER, tftMin);
			}
		}
	}
}

void COpenGLMaterialRenderComponent::setClearColor(SColor clearColor)
{
	DEVICE_SET_CLEARCOLOR(ClearColor, SColorf(clearColor));
}

void COpenGLMaterialRenderComponent::setViewport(recti vp)
{
	DEVICE_SET_VIEWPORT(Viewport, vp);
}

void COpenGLMaterialRenderComponent::setActiveTexture(uint32_t st)
{
	if (Driver->GLExtension.MaxTextureUnits > 1)
	{
		GLint v = (GLint)(GL_TEXTURE0_ARB + st);

		if (RsCache.ActiveTextureIndex != v)
		{
			Driver->GLExtension.extGlActiveTexture(v);
			RsCache.ActiveTextureIndex = v;
		}
	}
}

void COpenGLMaterialRenderComponent::setZWriteEnable(bool enable)
{
	CurrentRenderState.ZWriteEnable = enable ? GL_TRUE : GL_FALSE;
	DEVICE_SET_DEPTHMASK_STATE(ZWriteEnable, CurrentRenderState.ZWriteEnable);
}

bool COpenGLMaterialRenderComponent::getZWriteEnable() const
{
	return CurrentRenderState.ZWriteEnable != GL_FALSE;
}

void COpenGLMaterialRenderComponent::setTextureFilter(uint32_t st, E_TEXTURE_FILTER filter, bool mipmap, bool isCube)
{
	GLint tftMag, tftMin, tftMip;

	// Bilinear, trilinear, and anisotropic filter
	GLint maxAnisotropy = 1;
	if (filter != ETF_NONE)
	{
		uint8_t anisotropic = getAnisotropic(filter);
		tftMag = GL_LINEAR;
		tftMin = GL_LINEAR;
		tftMip = filter > ETF_BILINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST;

		if (Driver->GLExtension.queryOpenGLFeature(IRR_EXT_texture_filter_anisotropic))
			maxAnisotropy = min_(anisotropic, Driver->GLExtension.MaxAnisotropy);
	}
	else
	{
		tftMag = GL_NEAREST;
		tftMin = GL_NEAREST;
		tftMip = GL_NEAREST_MIPMAP_NEAREST;
	}

	if (!isCube)
	{
		DEVICE_SET_TEXTURE2D_PARAMETER_I(st, magFilter, GL_TEXTURE_MAG_FILTER, tftMag);

		if (mipmap)
		{
			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, mipFilter, GL_TEXTURE_MIN_FILTER, tftMip);
		}
		else
		{
			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, minFilter, GL_TEXTURE_MIN_FILTER, tftMin);
		}
		if (Driver->GLExtension.queryOpenGLFeature(IRR_EXT_texture_filter_anisotropic))
		{
			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, maxAniso, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
		}
	}
	else
	{
		DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, magFilter, GL_TEXTURE_MAG_FILTER, tftMag);

		if (mipmap)
		{
			DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, mipFilter, GL_TEXTURE_MIN_FILTER, tftMip);
		}
		else
		{
			DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, minFilter, GL_TEXTURE_MIN_FILTER, tftMin);
		}
		if (Driver->GLExtension.queryOpenGLFeature(IRR_EXT_texture_filter_anisotropic))
		{
			DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, maxAniso, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
		}
	}
}


void COpenGLMaterialRenderComponent::setTextureMipMap(uint32_t st, bool mipmap, bool isCube)
{
	GLint baselevel = 0;
	GLint maxlevel = mipmap ? 1000 : 0;

	if (!isCube)
	{
		DEVICE_SET_TEXTURE2D_PARAMETER_I(st, baseLevel, GL_TEXTURE_BASE_LEVEL, baselevel);
		DEVICE_SET_TEXTURE2D_PARAMETER_I(st, maxLevel, GL_TEXTURE_MAX_LEVEL, maxlevel);
	}
	else
	{
		DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, baseLevel, GL_TEXTURE_BASE_LEVEL, baselevel);
		DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, maxLevel, GL_TEXTURE_MAX_LEVEL, maxlevel);
	}
}

void COpenGLMaterialRenderComponent::setSamplerTexture(uint32_t st, ITexture* tex, bool isCube)
{
	setActiveTexture(st);

	COpenGLTexture* gltex = static_cast<COpenGLTexture*>(tex);
	glBindTexture(isCube ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, gltex ? gltex->getGLTexture() : 0);
	ASSERT_OPENGL_SUCCESS();
	RsCache.TextureUnits[st].texture = tex;
}

void COpenGLMaterialRenderComponent::setSamplerTextureMultiSample(uint32_t st, ITexture* tex)
{
	setActiveTexture(st);

	COpenGLTexture* gltex = static_cast<COpenGLTexture*>(tex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gltex ? gltex->getGLTexture() : 0);
	ASSERT_OPENGL_SUCCESS();
	RsCache.TextureUnits[st].texture = tex;
}

void COpenGLMaterialRenderComponent::setTextureWrap(uint32_t st, E_TEXTURE_ADDRESS address, E_TEXTURE_CLAMP wrap, bool isCube)
{
	ASSERT(RsCache.TextureUnits[st].texture);

	GLint v = COpenGLHelper::getGLTextureAddress(wrap);

	if (!isCube)
	{
		switch (address)
		{
		case ETA_U:
			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressS, GL_TEXTURE_WRAP_S, v);
			break;
		case ETA_V:
			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressT, GL_TEXTURE_WRAP_T, v);
			break;
		default:
			ASSERT(false);
		}
	}
	else
	{
		switch (address)
		{
		case ETA_U:
			DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, addressS, GL_TEXTURE_WRAP_S, v);
			break;
		case ETA_V:
			DEVICE_SET_TEXTURECUBE_PARAMETER_I(st, addressT, GL_TEXTURE_WRAP_T, v);
			break;
		default:
			ASSERT(false);
		}
	}
}

void COpenGLMaterialRenderComponent::resetRSCache()
{
	glGetFloatv(GL_COLOR_CLEAR_VALUE, (GLfloat*)&RsCache.ClearColor);

	GLint v[4];
	glGetIntegerv(GL_VIEWPORT, v);
	RsCache.Viewport.set(v[0], v[1], v[0] + v[2], v[1] + v[3]);

	RsCache.ZEnable = glIsEnabled(GL_DEPTH_TEST);
	glGetIntegerv(GL_DEPTH_FUNC, &RsCache.ZFunc);
	glGetBooleanv(GL_COLOR_WRITEMASK, RsCache.ColorMask);
	glGetBooleanv(GL_DEPTH_WRITEMASK, &RsCache.ZWriteEnable);
	//RsCache.StencilEnable = glIsEnabled(GL_STENCIL_TEST);
	//RsCache.ScissorEnable = glIsEnabled(GL_SCISSOR_TEST);
	RsCache.CullEnable = glIsEnabled(GL_CULL_FACE);
	glGetIntegerv(GL_CULL_FACE_MODE, &RsCache.CullMode);
	glGetIntegerv(GL_FRONT_FACE, &RsCache.FrontFace);
	RsCache.MultiSampleAntiAlias = glIsEnabled(GL_MULTISAMPLE_ARB);
	RsCache.AntiAliasedLineEnable = glIsEnabled(GL_LINE_SMOOTH);
	RsCache.AlphaBlendEnable = glIsEnabled(GL_BLEND);
	glGetIntegerv(GL_BLEND_SRC_RGB, &RsCache.SrcBlend);
	glGetIntegerv(GL_BLEND_DST_RGB, &RsCache.DestBlend);

	//texture op
	for (uint32_t i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		RsCache.TextureUnits[i].texture = nullptr;
	}

	glGetIntegerv(GL_ACTIVE_TEXTURE, &RsCache.ActiveTextureIndex);

	for (uint32_t idx = 0; idx < MATERIAL_MAX_TEXTURES; ++idx)
	{
		setActiveTexture(idx);

		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, &RsCache.TextureUnits[idx].baseLevel);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, &RsCache.TextureUnits[idx].maxLevel);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &RsCache.TextureUnits[idx].minFilter);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &RsCache.TextureUnits[idx].magFilter);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &RsCache.TextureUnits[idx].addressS);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &RsCache.TextureUnits[idx].addressT);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &RsCache.TextureUnits[idx].maxAniso);
	}
}
