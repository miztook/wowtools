#pragma once

#include "ITexture.h"
#include "COpenGLPCH.h"

class ITextureWriter;
class IImage;
class COpenGLDriver;

class COpenGLTexture : public ITexture
{
public:
	COpenGLTexture(const COpenGLDriver* driver, bool mipmap);
	~COpenGLTexture();

protected:
	//IVideoResource
	bool buildVideoResources() override;
	void releaseVideoResources() override;
	bool hasVideoBuilt() const override { return VideoBuilt; }

public:
	//ITexture
	bool createEmptyTexture(const dimension2d& size, ECOLOR_FORMAT format);
	bool createFromImage(std::shared_ptr<IImage> image);
	bool createRTTexture(const dimension2d& size, ECOLOR_FORMAT format, int nSamples);
	bool createDSTexture_INTZ(const dimension2d& size, int nSamples);

	GLuint getGLTexture() const { return GLTexture; }

private:
	bool createGLTexture(const dimension2d& size, ECOLOR_FORMAT format, bool mipmap, bool cube);

	bool internalCreateFromImage(const IImage* image);

	void copyTexture(const ITextureWriter* writer, const IImage* image);

	bool copyMipMaps(const ITextureWriter* writer, const IImage* image, uint32_t level = 1);

private:
	const COpenGLDriver*	Driver;
	GLuint		GLTexture;
	bool	VideoBuilt;
};