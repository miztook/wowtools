#pragma once

#include "base.h"
#include "vector2d.h"
#include "rect.h"

class ITexture;

//在d3d9中，纹理都是managed类型(lockable)，textureWriter是自身的一个代理
//在d3d9ex, d3d10, d3d11中，纹理必须是显存纹理(unlockable)，textureWriter就是专门用内存纹理写显存纹理的工具
class ITextureWriter
{
public:
	ITextureWriter(const vector2di& size, ECOLOR_FORMAT format, uint32_t numMipmap, bool cube)
		: TextureSize(size), ColorFormat(format), NumMipmaps(numMipmap), IsCube(cube)
	{
	}
	ITextureWriter()
		: TextureSize(0, 0), ColorFormat(ECF_UNKNOWN), NumMipmaps(1), IsCube(false)
	{
	}
	virtual ~ITextureWriter() {}

public:
	virtual void* lock(uint32_t face, uint32_t level, uint32_t& pitch) const = 0;
	virtual void unlock(uint32_t face, uint32_t level) const = 0;
	virtual bool copyToTexture(ITexture* texture, const recti* descRect = nullptr) const = 0;
	virtual void initEmptyData() = 0;

	uint32_t getNumFaces() const { return IsCube ? 6 : 1; }

public:
	vector2di	TextureSize;
	ECOLOR_FORMAT	ColorFormat;
	uint32_t		NumMipmaps;
	bool		IsCube;
};