#include "COpenGLRenderTarget.h"

#include "COpenGLDriver.h"
#include "COpenGLTexture.h"
#include "COpenGLHelper.h"

COpenGLRenderTarget::COpenGLRenderTarget(const COpenGLDriver* driver, const dimension2d& size, const SRTCreateParam& param, bool bUseDepthTexture)
	: IRenderTarget(size, param, bUseDepthTexture), Driver(driver), FrameBuffer(0), CopyFrameBuffer(0), DepthSurface(0),
	MultiSample(Driver->isMultiSampleEnabled()), VideoBuilt(false)
{
	//attachment
	for (int i = 0; i < NumColorAttachments; ++i)
		Attachments[i] = GL_COLOR_ATTACHMENT0 + i;
	if (HasDepthAttachment)
		Attachments[NumColorAttachments] = GL_DEPTH_ATTACHMENT;

	memset(ColorTextures, 0, sizeof(ColorTextures));
	memset(RTCopyTextures, 0, sizeof(RTCopyTextures));
	DepthTexture = nullptr;

	buildVideoResources();
}

COpenGLRenderTarget::~COpenGLRenderTarget()
{
	releaseVideoResources();
}

ITexture* COpenGLRenderTarget::getRTTexture(int index) const
{
	if (index < 0 || index >= NumColorAttachments)
	{
		ASSERT(false);
		return nullptr;
	}
	return MultiSample ? RTCopyTextures[index] : ColorTextures[index];
}

ITexture* COpenGLRenderTarget::getDepthTexture() const
{
	return DepthTexture;
}

bool COpenGLRenderTarget::writeToRTTexture()
{
	if (MultiSample)
	{
		ASSERT(CopyFrameBuffer);
		Driver->GLExtension.extGlBindFramebuffer(GL_READ_FRAMEBUFFER, FrameBuffer);
		Driver->GLExtension.extGlBindFramebuffer(GL_DRAW_FRAMEBUFFER, CopyFrameBuffer);
		Driver->GLExtension.extGlBlitFramebufferEXT(0, 0, TextureSize.width, TextureSize.height, 0, 0, TextureSize.width, TextureSize.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		if (Driver->GLExtension.DiscardFrameSupported)
		{
			GLsizei num = getnumAttachments();
			Driver->GLExtension.extGlInvalidateFramebuffer(GL_READ_FRAMEBUFFER, num, Attachments);
		}
		Driver->GLExtension.extGlBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		Driver->GLExtension.extGlBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	}

	return true;
}

bool COpenGLRenderTarget::bindFrameBuffer(bool bindDepth) const
{
	if (!VideoBuilt)
	{
		ASSERT(false);
		return false;
	}

	Driver->GLExtension.extGlBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
	//bind
	for (int i = 0; i < NumColorAttachments; ++i)
	{
		Driver->GLExtension.extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
			MultiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
			ColorTextures[i]->getGLTexture(), 0);
	}

	if (HasDepthAttachment && bindDepth)
	{
		if (UseDepthTexture)
			Driver->GLExtension.extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				MultiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
				DepthTexture->getGLTexture(), 0);
		else
			Driver->GLExtension.extGlFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthSurface);
	}

	Driver->GLExtension.extGlDrawBuffers(NumColorAttachments, Attachments);

	bool success = Driver->GLExtension.checkFBOStatus();
	if (!success)
	{
		Driver->GLExtension.extGlBindFramebuffer(GL_FRAMEBUFFER, 0);
		ASSERT(false);
		return false;
	}

	Driver->GLExtension.extGlBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

bool COpenGLRenderTarget::buildVideoResources()
{
	if (VideoBuilt)
		return false;

	const SDriverSetting& setting = Driver->getDriverSetting();
	bool success = createAsRenderTarget(TextureSize, ColorFormats, DepthFormat, setting.antialias);
	ASSERT(success);

	if (MultiSample)
	{
		for (int i = 0; i < NumColorAttachments; ++i)
		{
			ASSERT(!RTCopyTextures[i]);
			RTCopyTextures[i] = new COpenGLTexture(Driver, false);
			success = RTCopyTextures[i]->createEmptyTexture(TextureSize, ColorFormats[i]);
			ASSERT(success);
		}
		bindTexture();
	}

	VideoBuilt = success;
	return success;
}

void COpenGLRenderTarget::releaseVideoResources()
{
	if (!VideoBuilt)
		return;

	if (MultiSample)
	{
		for (int i = 0; i < NumColorAttachments; ++i)
		{
			if (RTCopyTextures[i])
			{
				delete RTCopyTextures[i];
				RTCopyTextures[i] = nullptr;
			}
		}
	}

	if (DepthSurface)
	{
		Driver->GLExtension.extGlDeleteRenderbuffers(1, &DepthSurface);
		DepthSurface = 0;
	}

	if (DepthTexture)
	{
		delete DepthTexture;
		DepthTexture = nullptr;
	}

	for (int i = 0; i < NumColorAttachments; ++i)
	{
		if (ColorTextures[i])
		{
			delete ColorTextures[i];
			ColorTextures[i] = nullptr;
		}
	}

	if (CopyFrameBuffer)
	{
		Driver->GLExtension.extGlDeleteFramebuffers(1, &CopyFrameBuffer);
		CopyFrameBuffer = 0;
	}

	if (FrameBuffer)
	{
		Driver->GLExtension.extGlDeleteFramebuffers(1, &FrameBuffer);
		FrameBuffer = 0;
	}

	VideoBuilt = false;
}

bool COpenGLRenderTarget::createAsRenderTarget(const dimension2d& size, ECOLOR_FORMAT colorFmts[MAX_COLOR_ATTACHMENTS], ECOLOR_FORMAT depthFmt, uint8_t antialias)
{
	for (int i = 0; i < NumColorAttachments; ++i)
	{
		ColorFormats[i] = (colorFmts[i] == ECF_UNKNOWN ? Driver->ColorFormat : colorFmts[i]);
	}

	DepthFormat = (depthFmt == ECF_UNKNOWN ? Driver->DepthFormat : depthFmt);

	//
	ASSERT(!FrameBuffer && !DepthSurface && !CopyFrameBuffer);
	ASSERT(Driver->GLExtension.MaxMultiSample >= antialias * 2);

	Driver->GLExtension.extGlGenFramebuffers(1, &FrameBuffer);
	Driver->GLExtension.extGlBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);

	if (MultiSample)
		Driver->GLExtension.extGlGenFramebuffers(1, &CopyFrameBuffer);

	//color
	for (int i = 0; i < NumColorAttachments; ++i)
	{
		ASSERT(!ColorTextures[i]);
		ColorTextures[i] = new COpenGLTexture(Driver, false);
		if (MultiSample)
			ColorTextures[i]->createRTTexture(size, ColorFormats[i], antialias * 2);
		else
			ColorTextures[i]->createRTTexture(size, ColorFormats[i], 0);
	}

	//depth
	if (HasDepthAttachment)
	{
		if (UseDepthTexture)		//texture
		{
			ASSERT(!DepthTexture);
			DepthTexture = new COpenGLTexture(Driver, false);
			bool bRet;
			if (MultiSample)
				bRet = DepthTexture->createDSTexture_INTZ(size, antialias * 2);
			else
				bRet = DepthTexture->createDSTexture_INTZ(size, 0);
			if (!bRet)
			{
				delete DepthTexture;
				DepthTexture = nullptr;
				UseDepthTexture = false;
			}
			else
			{
				DepthFormat = ECF_INTZ;
			}
		}

		if (!UseDepthTexture)
		{
			Driver->GLExtension.extGlGenRenderbuffers(1, &DepthSurface);
			GLenum dfmt = COpenGLHelper::getGLDepthFormat(DepthFormat);
			Driver->GLExtension.extGlBindRenderbuffer(GL_RENDERBUFFER, DepthSurface);
			if (MultiSample)
				Driver->GLExtension.extGlRenderbufferStorageMultisample(GL_RENDERBUFFER, antialias * 2, dfmt, size.width, size.height);
			else
				Driver->GLExtension.extGlRenderbufferStorage(GL_RENDERBUFFER, dfmt, size.width, size.height);
			Driver->GLExtension.extGlBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	}

	VideoBuilt = true;
	return true;
}

void COpenGLRenderTarget::bindTexture()
{
	if (MultiSample)
	{
		Driver->GLExtension.extGlBindFramebuffer(GL_FRAMEBUFFER, CopyFrameBuffer);

		for (int i = 0; i < NumColorAttachments; ++i)
		{
			Driver->GLExtension.extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, RTCopyTextures[i]->getGLTexture(), 0);
		}

		bool success = Driver->GLExtension.checkFBOStatus();
		ASSERT(success);

		Driver->GLExtension.extGlBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}