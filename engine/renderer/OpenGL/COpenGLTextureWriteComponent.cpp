#include "COpenGLTextureWriteComponent.h"

#include "COpenGLDriver.h"
#include "COpenGLHelper.h"
#include "ITexture.h"
#include "COpenGLMaterialRenderComponent.h"

COpenGLTextureWriter::COpenGLTextureWriter(COpenGLDriver* driver, const dimension2d& size, ECOLOR_FORMAT format, uint32_t numMipmap, bool cube)
	: ITextureWriter(size, format, numMipmap, cube), Driver(driver)
{
	uint32_t numFaces = getNumFaces();

	MipData = new SMipData[numFaces * NumMipmaps];
	for (uint32_t face = 0; face < numFaces; ++face)
	{
		for (uint32_t i = 0; i < NumMipmaps; ++i)
		{
			dimension2d mipsize = size.getMipLevelSize(i);

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

bool COpenGLTextureWriter::copyToTexture(const ITexture* texture, const recti * descRect) const
{
	COpenGLMaterialRenderComponent* materialRenderComponent = Driver->getMaterialRenderComponent();
	GLenum pixelFormat;
	GLenum pixelType;

	GLint internalFormat = COpenGLHelper::getGLTextureFormat(ColorFormat, pixelFormat, pixelType);
	ASSERT(texture->getType() == ETT_IMAGE && texture->getSampleCount() == 0);

	bool compressed = isCompressedFormat(ColorFormat);
	bool cube = texture->isCube();

	if (descRect)
	{
		ASSERT(texture->getNumMipmaps() == 1);

		GLint left = descRect->left;
		GLint top = descRect->top;
		GLint width = descRect->getWidth();
		GLint height = descRect->getHeight();

		uint32_t bpp = getBytesPerPixelFromFormat(ColorFormat);

		//begin bind
		materialRenderComponent->setSamplerTexture(0, texture, cube);
		materialRenderComponent->setTextureFilter(0, ETF_NONE, false, cube);
		materialRenderComponent->setTextureWrap(0, ETA_U, ETC_CLAMP, cube);
		materialRenderComponent->setTextureWrap(0, ETA_V, ETC_CLAMP, cube);
		materialRenderComponent->setTextureMipMap(0, texture->getNumMipmaps() > 1, cube);

		uint32_t numFaces = getNumFaces();
		for (uint32_t face = 0; face < numFaces; ++face)
		{
			const SMipData* mipData = getMipData(face, 0);
			const uint8_t* start = mipData->data + mipData->pitch * top + bpp * left;
			uint8_t* data = new uint8_t[bpp * width * height];

			uint8_t* dst = data;
			uint8_t* src = (uint8_t*)start;
			for (int h = 0; h < height; ++h)
			{
				Q_memcpy(dst, bpp * width, src, bpp * width);
				dst += bpp * width;
				src += mipData->pitch;
			}

			GLenum nTarget = cube ? (GL_TEXTURE_CUBE_MAP_POSITIVE_X + face) : GL_TEXTURE_2D;
			if (!compressed)
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);			//必须，否则默认为4字节对齐，r8g8b8纹理不处理会出错
				glTexSubImage2D(nTarget, 0, left, top, width, height, pixelFormat, pixelType, data);
				ASSERT_OPENGL_SUCCESS();
			}
			else
			{
				glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
				uint32_t pitch, compressedSize;
				getImagePitchAndBytes(ColorFormat, width, height, pitch, compressedSize);
				ASSERT(width > 0 && height > 0);
				Driver->GLExtension.extGlCompressedTexSubImage2D(nTarget, 0, left, top, width, height, internalFormat, compressedSize, data);
			}

			delete[] data;

			//end bind
			materialRenderComponent->setSamplerTexture(0, nullptr, cube);
		}
	}
	else
	{
		//begin bind
		materialRenderComponent->setSamplerTexture(0, texture, cube);
		materialRenderComponent->setTextureFilter(0, ETF_NONE, false, cube);
		materialRenderComponent->setTextureWrap(0, ETA_U, ETC_CLAMP, cube);
		materialRenderComponent->setTextureWrap(0, ETA_V, ETC_CLAMP, cube);
		materialRenderComponent->setTextureMipMap(0, texture->getNumMipmaps() > 1, cube);

		uint32_t numFaces = getNumFaces();
		for (uint32_t face = 0; face < numFaces; ++face)
		{
			for (uint32_t i = 0; i < NumMipmaps; ++i)
			{
				const SMipData* mipData = getMipData(face, i);
				dimension2d size = TextureSize.getMipLevelSize(i);
				GLenum nTarget = cube ? (GL_TEXTURE_CUBE_MAP_POSITIVE_X + face) : GL_TEXTURE_2D;
				if (!compressed)
				{
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);			//必须，否则默认为4字节对齐，r8g8b8纹理不处理会出错
					glTexImage2D(nTarget, i, internalFormat, size.width, size.height, 0, pixelFormat, pixelType, mipData->data);
					ASSERT_OPENGL_SUCCESS();
				}
				else
				{
					glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
					uint32_t pitch, compressedSize;
					getImagePitchAndBytes(ColorFormat, size.width, size.height, pitch, compressedSize);
					ASSERT(size.width > 0 && size.height > 0);
					Driver->GLExtension.extGlCompressedTexImage2D(nTarget, i, internalFormat, size.width, size.height, 0, compressedSize, mipData->data);
				}
			}
		}

		//end bind
		materialRenderComponent->setSamplerTexture(0, nullptr, cube);
	}

	return true;
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
			ASSERT(false);
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

COpenGLTextureWriteComponent::COpenGLTextureWriteComponent(COpenGLDriver* driver)
	: Driver(driver)
{

}

COpenGLTextureWriteComponent::~COpenGLTextureWriteComponent()
{
	for (const auto& kv : TextureWriterMap)
	{
		delete kv.second;
	}
	TextureWriterMap.clear();
}

bool COpenGLTextureWriteComponent::init()
{
	return true;
}

ITextureWriter* COpenGLTextureWriteComponent::createTextureWriter(const ITexture* texture)
{
	ECOLOR_FORMAT format = texture->getColorFormat();
	const dimension2d& size = texture->getSize();
	uint32_t numMipmap = texture->getNumMipmaps();
	bool cube = texture->isCube();

	auto itr = TextureWriterMap.find(texture);
	if (itr != TextureWriterMap.end())
	{
		return itr->second;
	}

	//not found, create
	COpenGLTextureWriter* writer = new COpenGLTextureWriter(Driver, size, format, numMipmap, cube);
	TextureWriterMap[texture] = writer;

	return writer;
}

bool COpenGLTextureWriteComponent::removeTextureWriter(const ITexture* texture)
{
	auto itr = TextureWriterMap.find(texture);
	if (itr != TextureWriterMap.end())
	{
		delete itr->second;
		TextureWriterMap.erase(itr);
		return true;
	}

	return false;
}