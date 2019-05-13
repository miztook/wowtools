#include "COpenGLTextureWriteComponent.h"

#include "COpenGLDriver.h"
#include "COpenGLHelper.h"

COpenGLTextureWriter::COpenGLTextureWriter(COpenGLDriver* driver, const vector2di& size, ECOLOR_FORMAT format, uint32_t numMipmap, bool cube)
	: ITextureWriter(size, format, numMipmap, cube), Driver(driver)
{
	uint32_t numFaces = getNumFaces();

	MipData = new SMipData[numFaces * NumMipmaps];
	for (uint32_t face = 0; face < numFaces; ++face)
	{
		for (uint32_t i = 0; i < NumMipmaps; ++i)
		{
			vector2di mipsize = size.getMipLevelSize(i);

			uint32_t pitch, bytes;
			getImagePitchAndBytes(ColorFormat, mipsize.width, mipsize.height, pitch, bytes);
			bytes = ((bytes + 3) & ~3);

			MipData[face*NumMipmaps + i].pitch = pitch;
			MipData[face*NumMipmaps + i].data = new uint8_t[bytes];
		}
	}
}

COpenGLTextureWriter::~COpenGLTextureWriter()
{
	uint32_t numFaces = getNumFaces();
	for (uint32_t i = 0; i < numFaces * NumMipmaps; ++i)
	{
		delete[] MipData[i].data;
	}
	delete[] MipData;
}

void* COpenGLTextureWriter::lock(uint32_t face, uint32_t level, uint32_t& pitch) const
{
	if (level >= NumMipmaps)
		return nullptr;

	const SMipData* mipData = getMipData(face, level);

	pitch = mipData->pitch;
	return mipData->data;
}

void COpenGLTextureWriter::unlock(uint32_t face, uint32_t level) const
{
}

void COpenGLTextureWriter::initEmptyData()
{
	uint32_t numFaces = getNumFaces();
	for (uint32_t face = 0; face < numFaces; ++face)
	{
		uint32_t pitch;
		void* dest = lock(face, 0, pitch);
		if (!dest)
		{
			assert(false);
			continue;
		}
		memset(dest, 0, pitch * TextureSize.height);
		unlock(face, 0);
	}
}

const COpenGLTextureWriter::SMipData* COpenGLTextureWriter::getMipData(uint32_t face, uint32_t mipLevel) const
{
	return &MipData[face * NumMipmaps + mipLevel];
}
