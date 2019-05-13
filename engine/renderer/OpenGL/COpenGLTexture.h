#pragma once

#include "ITexture.h"
#include "COpenGLPCH.h"

class ITextureWriter;
class IImage;
class COpenGLDriver;

class COpenGLTexture : public ITexture
{
public:
	COpenGLTexture(COpenGLDriver* driver, bool mipmap);
	~COpenGLTexture();

protected:
	//IVideoResource
	virtual bool buildVideoResources() override;
	virtual void releaseVideoResources() override;
	virtual bool hasVideoBuilt() const override { return VideoBuilt; }

public:
	//ITexture
	bool createEmptyTexture(const vector2di& size, ECOLOR_FORMAT format);
	bool createFromImage(std::shared_ptr<IImage> image);
	bool createRTTexture(const vector2di& size, ECOLOR_FORMAT format, int nSamples);
	bool createDSTexture_INTZ(const vector2di& size, int nSamples);

	GLuint getGLTexture() const { return GLTexture; }

private:
	bool createGLTexture(const vector2di& size, ECOLOR_FORMAT format, bool mipmap, bool cube);

	bool internalCreateFromImage(const IImage* image);

	void copyTexture(const ITextureWriter* writer, const IImage* image);

	bool copyMipMaps(const ITextureWriter* writer, const IImage* image, uint32_t level = 1);

private:
	COpenGLDriver*	Driver;
	GLuint		GLTexture;
};