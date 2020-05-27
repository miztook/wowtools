#pragma once

#include "COpenGLPCH.h"
#include "IRenderTarget.h"

class COpenGLTexture;
class COpenGLDriver;

class COpenGLRenderTarget : public IRenderTarget
{
public:
	COpenGLRenderTarget(COpenGLDriver* driver, const dimension2d& size, ECOLOR_FORMAT colorFmt)
		: COpenGLRenderTarget(driver, size, SRTCreateParam(colorFmt), false)
	{
	}

	COpenGLRenderTarget(COpenGLDriver* driver, const dimension2d& size, ECOLOR_FORMAT colorFmt, ECOLOR_FORMAT depthFmt, bool bDepthTexture)
		: COpenGLRenderTarget(driver, size, SRTCreateParam(colorFmt, depthFmt), bDepthTexture)
	{
	}

	COpenGLRenderTarget(COpenGLDriver* driver, const dimension2d& size, ECOLOR_FORMAT colorFmts[3], ECOLOR_FORMAT depthFmt, bool bDepthTexture)
		: COpenGLRenderTarget(driver, size, SRTCreateParam(colorFmts, depthFmt), bDepthTexture)
	{
	}

	COpenGLRenderTarget(const COpenGLDriver* driver, const dimension2d& size, const SRTCreateParam& param, bool bUseDepthTexture);
	~COpenGLRenderTarget();

public:
	bool isValid() const override { return FrameBuffer != 0; }
	const ITexture* getRTTexture(int index) const override;
	const ITexture* getDepthTexture() const override;
	bool writeToRTTexture() override;

	GLuint getFrameBuffer() const { return FrameBuffer; }
	const GLenum* getAttachments() const { return Attachments; }
	GLsizei getnumAttachments() const { return HasDepthAttachment ? NumColorAttachments + 1 : NumColorAttachments; }

	bool bindFrameBuffer(bool bindDepth) const;

protected:
	bool buildVideoResources() override;
	void releaseVideoResources() override;
	bool hasVideoBuilt() const override { return VideoBuilt; }

private:
	bool createAsRenderTarget(
		const dimension2d& size,
		ECOLOR_FORMAT colorFmts[MAX_COLOR_ATTACHMENTS],
		ECOLOR_FORMAT depthFmt,
		uint8_t antialias);

	void bindTexture();

private:
	const COpenGLDriver*	Driver;
	GLenum	Attachments[MAX_COLOR_ATTACHMENTS + 1];

	std::unique_ptr<COpenGLTexture>		RTCopyTextures[MAX_COLOR_ATTACHMENTS];
	GLuint		CopyFrameBuffer;

	GLuint		FrameBuffer;
	std::unique_ptr<COpenGLTexture>		ColorTextures[MAX_COLOR_ATTACHMENTS];
	std::unique_ptr<COpenGLTexture>		DepthTexture;
	GLuint		DepthSurface;

	const bool  MultiSample;
	bool	VideoBuilt;
};