#include "COpenGLTexture.h"

#include "COpenGLDriver.h"
#include "COpenGLHelper.h"
#include "COpenGLMaterialRenderComponent.h"
#include "COpenGLTextureWriteComponent.h"
#include "CFileSystem.h"

COpenGLTexture::COpenGLTexture(const COpenGLDriver* driver, bool mipmap)
	: ITexture(mipmap), Driver(driver), GLTexture(0)
{
	VideoBuilt = false;
}

COpenGLTexture::~COpenGLTexture()
{
	releaseVideoResources();
}

bool COpenGLTexture::createEmptyTexture(const dimension2d& size, ECOLOR_FORMAT format)
{
	//CLock lock(&g_Globals.textureCS);

	if (VideoBuilt || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	ASSERT(!HasMipMaps);

	NumMipmaps = 1;
	TextureSize = size;
	ColorFormat = format;
	IsCube = false;

	if (!createGLTexture(TextureSize, ColorFormat, HasMipMaps, IsCube))
	{
		g_FileSystem->writeLog(ELOG_RES, "COpenGLTexture::createEmptyTexture Failed");
		ASSERT(false);
		return false;
	}

	if (!HasMipMaps)
	{
		ITextureWriter* texWriter = Driver->getTextureWriteComponent()->createTextureWriter(this);
		texWriter->initEmptyData();
		texWriter->copyToTexture(this);
		Driver->getTextureWriteComponent()->removeTextureWriter(this);
	}

	VideoBuilt = true;

	return true;
}

bool COpenGLTexture::createFromImage(std::shared_ptr<IImage> image)
{
	//CLock lock(&g_Globals.textureCS);

	ASSERT(Type == ETT_IMAGE);

	if (VideoBuilt)
		return true;

	if (Type != ETT_IMAGE || !image || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	NumMipmaps = HasMipMaps ? image->getDimension().getNumMipLevels() : 1;
	TextureSize = image->getDimension();
	ColorFormat = image->getColorFormat();
	IsCube = image->isCube();

	if (!internalCreateFromImage(image.get()))
	{
		ASSERT(false);
		return false;
	}

	VideoBuilt = true;

	return true;
}

bool COpenGLTexture::createRTTexture(const dimension2d& size, ECOLOR_FORMAT format, int nSamples)
{
	//CLock lock(&g_Globals.textureCS);

	if (VideoBuilt || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	ASSERT(!HasMipMaps);

	Type = ETT_RENDERTARGET;
	NumMipmaps = 1;
	TextureSize = size;
	ColorFormat = format;
	SampleCount = nSamples;

	glGenTextures(1, &GLTexture);
	ASSERT_OPENGL_SUCCESS();

	COpenGLMaterialRenderComponent* materialRenderComponent = Driver->getMaterialRenderComponent();
	GLenum pixelFormat;
	GLenum pixelType;

	GLint internalFormat = COpenGLHelper::getGLTextureFormat(ColorFormat, pixelFormat, pixelType);

	if (SampleCount == 0)
	{
		//begin bind
		materialRenderComponent->setSamplerTexture(0, this, false);
		materialRenderComponent->setTextureFilter(0, ETF_NONE, false, false);
		materialRenderComponent->setTextureWrap(0, ETA_U, ETC_CLAMP, false);
		materialRenderComponent->setTextureWrap(0, ETA_V, ETC_CLAMP, false);
		materialRenderComponent->setTextureMipMap(0, false, false);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.width, size.height, 0, pixelFormat, pixelType, nullptr);
		ASSERT_OPENGL_SUCCESS();

		//end bind
		materialRenderComponent->setSamplerTexture(0, nullptr, false);
	}
	else
	{
		materialRenderComponent->setSamplerTextureMultiSample(0, this);

		Driver->GLExtension.extGlTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SampleCount, internalFormat, size.width, size.width, GL_TRUE);

		materialRenderComponent->setSamplerTextureMultiSample(0, 0);
	}

	VideoBuilt = true;

	return true;
}

bool COpenGLTexture::createDSTexture_INTZ(const dimension2d& size, int nSamples)
{
	if (VideoBuilt || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	if (!Driver->isSupportDepthTexture())
		return false;

	ASSERT(!HasMipMaps);

	Type = ETT_DEPTHSTENCIL;
	NumMipmaps = 1;
	TextureSize = size;
	ColorFormat = ECF_INTZ;
	SampleCount = nSamples;

	glGenTextures(1, &GLTexture);
	ASSERT_OPENGL_SUCCESS();

	COpenGLMaterialRenderComponent* materialRenderComponent = Driver->getMaterialRenderComponent();
	GLenum pixelFormat;
	GLenum pixelType;
	GLint internalFormat = COpenGLHelper::getGLTextureFormat(ColorFormat, pixelFormat, pixelType);

	if (SampleCount == 0)
	{
		//begin bind
		materialRenderComponent->setSamplerTexture(0, this, false);
		materialRenderComponent->setTextureFilter(0, ETF_NONE, false, false);
		materialRenderComponent->setTextureWrap(0, ETA_U, ETC_CLAMP, false);
		materialRenderComponent->setTextureWrap(0, ETA_V, ETC_CLAMP, false);
		materialRenderComponent->setTextureMipMap(0, false, false);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.width, size.height, 0, pixelFormat, pixelType, nullptr);
		ASSERT_OPENGL_SUCCESS();

		//end bind
		materialRenderComponent->setSamplerTexture(0, nullptr, false);
	}
	else
	{
		materialRenderComponent->setSamplerTextureMultiSample(0, this);

		Driver->GLExtension.extGlTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, SampleCount, internalFormat, size.width, size.height, GL_TRUE);

		materialRenderComponent->setSamplerTextureMultiSample(0, nullptr);
	}

	VideoBuilt = true;

	return true;
}

bool COpenGLTexture::buildVideoResources()
{
	//CLock lock(&g_Globals.textureCS);

	if (VideoBuilt || Type != ETT_IMAGE)
		return true;

	if (!m_pImage || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	if (!internalCreateFromImage(m_pImage.get()))
	{
		ASSERT(false);
		return false;
	}

	m_pImage.reset();

	VideoBuilt = true;

	return true;
}

void COpenGLTexture::releaseVideoResources()
{
	//CLock lock(&g_Globals.textureCS);

	if (VideoBuilt)
	{
		if (GLTexture)
		{
			glDeleteTextures(1, &GLTexture);
			GLTexture = 0;
		}
		VideoBuilt = false;
	}
}

bool COpenGLTexture::createGLTexture(const dimension2d& size, ECOLOR_FORMAT format, bool mipmap, bool cube)
{
	if (VideoBuilt || GLTexture)
	{
		ASSERT(false);
		return false;
	}

	glGenTextures(1, &GLTexture);
	ASSERT_OPENGL_SUCCESS();

	if (!mipmap)
	{
		COpenGLMaterialRenderComponent* materialRenderComponent = Driver->getMaterialRenderComponent();

		GLenum pixelFormat;
		GLenum pixelType;

		uint32_t numFaces = cube ? 6 : 1;
		bool compressed = isCompressedFormat(ColorFormat);
		GLint internalFormat = COpenGLHelper::getGLTextureFormat(ColorFormat, pixelFormat, pixelType);

		//begin bind
		materialRenderComponent->setSamplerTexture(0, this, cube);
		materialRenderComponent->setTextureFilter(0, ETF_NONE, false, cube);
		materialRenderComponent->setTextureWrap(0, ETA_U, ETC_CLAMP, cube);
		materialRenderComponent->setTextureWrap(0, ETA_V, ETC_CLAMP, cube);
		materialRenderComponent->setTextureMipMap(0, false, cube);

		for (uint32_t face = 0; face < numFaces; ++face)
		{
			GLenum nTarget = cube ? (GL_TEXTURE_CUBE_MAP_POSITIVE_X + face) : GL_TEXTURE_2D;

			if (!compressed)
			{
				glTexImage2D(nTarget, 0, internalFormat, size.width, size.height, 0, pixelFormat, pixelType, nullptr);
				ASSERT_OPENGL_SUCCESS();
			}
			else
			{
				uint32_t pitch, compressedSize;
				getImagePitchAndBytes(ColorFormat, size.width, size.height, pitch, compressedSize);
				ASSERT(size.width > 0 && size.height > 0);
				Driver->GLExtension.extGlCompressedTexImage2D(nTarget, 0, internalFormat, size.width, size.height, 0, compressedSize, nullptr);
			}
		}

		//end bind
		materialRenderComponent->setSamplerTexture(0, nullptr, cube);
	}

	return true;
}

bool COpenGLTexture::internalCreateFromImage(const IImage* image)
{
	ASSERT(NumMipmaps == 1 ||
		image->getDimension().getNumMipLevels() == image->getNumMipLevels());
	ASSERT(TextureSize == image->getDimension() &&
		ColorFormat == image->getColorFormat() &&
		IsCube == image->isCube());

	if (!createGLTexture(TextureSize, ColorFormat, HasMipMaps, IsCube))
	{
		g_FileSystem->writeLog(ELOG_RES, "COpenGLTexture::internalCreateFromImage Failed");
		ASSERT(false);

		return false;
	}

	ITextureWriter* texWriter = Driver->getTextureWriteComponent()->createTextureWriter(this);

	copyTexture(texWriter, image);

	if (HasMipMaps)
		copyMipMaps(texWriter, image);

	texWriter->copyToTexture(this);

	Driver->getTextureWriteComponent()->removeTextureWriter(this);

	return true;
}

void COpenGLTexture::copyTexture(const ITextureWriter* writer, const IImage* image)
{
	ASSERT(image);

	uint32_t numFaces = writer->getNumFaces();
	for (uint32_t face = 0; face < numFaces; ++face)
	{
		uint32_t pitch;
		void* destData = writer->lock(face, 0, pitch);

		bool result = image->copyMipmapData(face, 0, destData, pitch, TextureSize.width, TextureSize.height);
		ASSERT(result);

		writer->unlock(face, 0);
	}
}

bool COpenGLTexture::copyMipMaps(const ITextureWriter* writer, const IImage* image, uint32_t level /*= 1*/)
{
	if (!HasMipMaps || !image)
		return false;

	if (level == 0 || level >= NumMipmaps)
		return true;

	dimension2d size = TextureSize.getMipLevelSize(level);

	bool result = false;
	uint32_t numFaces = writer->getNumFaces();
	for (uint32_t face = 0; face < numFaces; ++face)
	{
		//lock
		uint32_t pitch;
		void* destData = writer->lock(face, level, pitch);

		result = image->copyMipmapData(face, level, destData, pitch, size.width, size.height);
		ASSERT(result);

		// unlock
		writer->unlock(face, level);
	}

	if (!result || (size.width < 1 || size.height < 1))
		return false; // stop generating levels

					  // generate next level
	return copyMipMaps(writer, image, level + 1);
}