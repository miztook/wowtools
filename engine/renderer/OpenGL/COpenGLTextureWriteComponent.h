#pragma once

#include "ITextureWriter.h"
#include <map>

class COpenGLDriver;

class COpenGLTextureWriter : public ITextureWriter
{
private:
	COpenGLTextureWriter(COpenGLDriver* driver, const vector2di& size, ECOLOR_FORMAT format, uint32_t numMipmap, bool cube);
	~COpenGLTextureWriter();

	friend class COpenGLTextureWriteServices;

public:
	virtual void* lock(uint32_t face, uint32_t level, uint32_t& pitch) const;
	virtual void unlock(uint32_t face, uint32_t level) const;
	virtual bool copyToTexture(ITexture* texture, const recti* descRect = nullptr) const;
	virtual void initEmptyData();

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
	ITextureWriter* createTextureWriter(ITexture* texture, bool temp);
	bool removeTextureWriter(ITexture* texture);

private:
	std::map<ITexture*, ITextureWriter*>		TextureWriterMap;
	COpenGLDriver*	Driver;
};