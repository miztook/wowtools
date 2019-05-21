#pragma once

#include "ITextureWriter.h"
#include <map>

class COpenGLDriver;

class COpenGLTextureWriter : public ITextureWriter
{
private:
	COpenGLTextureWriter(COpenGLDriver* driver, const dimension2d& size, ECOLOR_FORMAT format, uint32_t numMipmap, bool cube);
	~COpenGLTextureWriter();

	friend class COpenGLTextureWriteComponent;

public:
	void* lock(uint32_t face, uint32_t level, uint32_t& pitch) const override;
	void unlock(uint32_t face, uint32_t level) const override;
	bool copyToTexture(ITexture* texture, const recti* descRect = nullptr) const override;
	void initEmptyData() override;

private:
	struct SMipData
	{
		uint32_t pitch;
		uint8_t* data;
	};

	const SMipData*	getMipData(uint32_t face, uint32_t mipLevel) const;

private:
	COpenGLDriver*	Driver;

	SMipData*		MipData;
};

class COpenGLTextureWriteComponent
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGLTextureWriteComponent);

public:
	explicit COpenGLTextureWriteComponent(COpenGLDriver* driver);
	~COpenGLTextureWriteComponent();

public:
	bool init();

	ITextureWriter* createTextureWriter(ITexture* texture);
	bool removeTextureWriter(ITexture* texture);

private:
	std::map<ITexture*, ITextureWriter*>		TextureWriterMap;
	COpenGLDriver*	Driver;
};