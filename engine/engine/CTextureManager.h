#pragma once

#pragma once

#include "base.h"
#include <memory>
#include <map>
#include <list>
#include "vector2d.h"
#include "CResourceCache.h"

class ITexture;
class IRenderTarget;
class IImage;
class wowEnvironment;

class CTextureManager
{
private:
	DISALLOW_COPY_AND_ASSIGN(CTextureManager);

public:
	explicit CTextureManager(wowEnvironment* wowEnv);
	~CTextureManager();

public:
	std::shared_ptr<IImage> loadImage(const char* filename);

	std::shared_ptr<IImage> loadBLP(const char* filename);

	std::shared_ptr<ITexture> loadTexture(const char* filename, bool mipmap);

public:
	std::shared_ptr<ITexture> getTextureWhite() const
	{
		return getManualTexture("$DefaultWhite");
	}

	std::shared_ptr<ITexture> getManualTexture(const char* name) const;
	std::shared_ptr<ITexture> addTexture(const char* name, std::shared_ptr<IImage> image, bool mipmap);
	void removeTexture(const char* name);

private:
	void loadDefaultTextures();

private:
	wowEnvironment*		WowEnv;

	std::map<std::string, std::shared_ptr<ITexture>>	TextureMap;
	std::list<IRenderTarget*>	RenderTargets;

	CResourceCache<IImage>	m_BlpImageCache;
	CResourceCache<ITexture>	m_TextureCache;
};