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
	ITexture* getTextureWhite() const { return DefaultWhite; }

	ITexture* getManualTexture(const char* name) const;
	ITexture* addTexture(const char* name, std::shared_ptr<IImage> image, bool mipmap);
	void removeTexture(const char* name);

	ITexture* createEmptyTexture(const dimension2d& size, ECOLOR_FORMAT format);
	ITexture* createTexture(bool mipmap, std::shared_ptr<IImage> image);

private:
	void loadDefaultTextures();

private:
	wowEnvironment*		WowEnv;

	ITexture*	DefaultWhite;
	std::map<std::string, ITexture*>	TextureMap;
	std::list<IRenderTarget*>	RenderTargets;

	CResourceCache<IImage>	m_BlpImageCache;
	CResourceCache<ITexture>	m_TextureCache;
};