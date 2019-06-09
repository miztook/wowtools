#include "COpenGLTextureManageComponent.h"

#include "COpenGLTexture.h"
#include "COpenGLRenderTarget.h"
#include "CCImage.h"

COpenGLTextureManageComponent::COpenGLTextureManageComponent(const COpenGLDriver* driver)
	: Driver(driver)
{

}

COpenGLTextureManageComponent::~COpenGLTextureManageComponent()
{
	for (auto itr = TextureMap.begin(); itr != TextureMap.end(); ++itr)
	{
		ITexture* tex = itr->second;
		if (tex)
			delete tex;
	}
	TextureMap.clear();
}

bool COpenGLTextureManageComponent::init()
{
	loadDefaultTextures();

	return true;
}

ITexture* COpenGLTextureManageComponent::getManualTexture(const char* name) const
{
	auto itr = TextureMap.find(name);
	if (itr == TextureMap.end())
		return nullptr;

	return itr->second;
}

ITexture* COpenGLTextureManageComponent::addTexture(const char* name, std::shared_ptr<CCImage> image, bool mipmap)
{
	if (TextureMap.find(name) != TextureMap.end())
		return nullptr;

	COpenGLTexture* tex = new COpenGLTexture(Driver, mipmap);
	if (!tex->createFromImage(image))
	{
		delete tex;
		return nullptr;
	}
	TextureMap[name] = tex;
	return tex;
}

void COpenGLTextureManageComponent::removeTexture(const char* name)
{
	auto itr = TextureMap.find(name);
	if (itr == TextureMap.end())
		return;

	ITexture* tex = itr->second;

	if (tex)
		delete tex;

	TextureMap.erase(itr);
}

void COpenGLTextureManageComponent::loadDefaultTextures()
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
