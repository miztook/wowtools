#include "CTextureManager.h"
#include "Engine.h"
#include "IVideoDriver.h"
#include "ITexture.h"
#include "CCImage.h"

CTextureManager::CTextureManager()
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
}

ITexture* CTextureManager::getManualTexture(const char* name) const
{
	auto itr = TextureMap.find(name);
	if (itr == TextureMap.end())
		return nullptr;

	return itr->second;
}

ITexture* CTextureManager::addTexture(const char* name, std::shared_ptr<CCImage> image, bool mipmap)
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
