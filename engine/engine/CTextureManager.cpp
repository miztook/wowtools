#include "CTextureManager.h"
#include "Engine.h"
#include "IVideoDriver.h"
#include "ITexture.h"
#include "CCImage.h"
#include "wowEnvironment.h"
#include "CBLPImage.h"
#include "CMemFile.h"

CTextureManager::CTextureManager(wowEnvironment* wowEnv)
	: WowEnv(wowEnv)
{
	DefaultWhite = nullptr;

	loadDefaultTextures();
}

CTextureManager::~CTextureManager()
{
	for (auto itr = TextureMap.begin(); itr != TextureMap.end(); ++itr)
	{
		ITexture* tex = itr->second;
		if (tex)
			delete tex;
	}
	TextureMap.clear();

	m_BlpImageCache.flushCache();
}

std::shared_ptr<IImage> CTextureManager::loadImage(const char* filename)
{
	if (hasFileExtensionA(filename, "blp"))
		return loadBLP(filename);
	else
		return nullptr;
}

std::shared_ptr<IImage> CTextureManager::loadBLP(const char* filename)
{
	if (strlen(filename) == 0)
		return nullptr;

	char realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	std::shared_ptr<IImage> image = m_BlpImageCache.tryLoadFromCache(realfilename);
	if (image)
		return image;
	
	CMemFile* file = WowEnv->openFile(realfilename);
	if (!file)
		return nullptr;

	std::shared_ptr<CBLPImage> blpImage(new CBLPImage);
	if (!blpImage->loadFile(file))
	{
		blpImage.reset();
		return nullptr;
	}

	delete file;

	if (blpImage)
		m_BlpImageCache.addToCache(realfilename, blpImage);

	return blpImage;
}

ITexture* CTextureManager::getManualTexture(const char* name) const
{
	auto itr = TextureMap.find(name);
	if (itr == TextureMap.end())
		return nullptr;

	return itr->second;
}

ITexture* CTextureManager::addTexture(const char* name, std::shared_ptr<IImage> image, bool mipmap)
{
	if (TextureMap.find(name) != TextureMap.end())
		return nullptr;

	ITexture* tex = g_Engine->getDriver()->createTexture(false, image);
	if (!tex)
	{
		return nullptr;
	}
	TextureMap[name] = tex;
	return tex;
}

void CTextureManager::removeTexture(const char* name)
{
	auto itr = TextureMap.find(name);
	if (itr == TextureMap.end())
		return;

	ITexture* tex = itr->second;
	if (tex)
		delete tex;

	TextureMap.erase(itr);
}

ITexture* CTextureManager::createEmptyTexture(const dimension2d& size, ECOLOR_FORMAT format)
{
	ITexture* tex = g_Engine->getDriver()->createTexture(false, size, format);
	return tex;
}

void CTextureManager::loadDefaultTextures()
{
	ECOLOR_FORMAT format = ECF_R5G6B5;
	dimension2d size(32, 32);

	uint32_t pitch, nbytes;
	getImagePitchAndBytes(format, size.width, size.height, pitch, nbytes);

	uint8_t* data = new uint8_t[nbytes];
	memset(data, 0xff, nbytes);

	std::shared_ptr<CCImage> img = std::make_shared<CCImage>(format, size, data, true);
	DefaultWhite = addTexture("$DefaultWhite", img, true);
}
