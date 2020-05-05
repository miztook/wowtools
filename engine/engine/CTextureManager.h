#pragma once

#pragma once

#include "base.h"
#include <memory>
#include <map>
#include <list>
#include "vector2d.h"

class ITexture;
class IRenderTarget;
class CCImage;

class CTextureManager
{
private:
	DISALLOW_COPY_AND_ASSIGN(CTextureManager);

public:
	CTextureManager();
	~CTextureManager();

public:
	ITexture* getTextureWhite() const { return DefaultWhite; }

	ITexture* getManualTexture(const char* name) const;
	ITexture* addTexture(const char* name, std::shared_ptr<CCImage> image, bool mipmap);
	void removeTexture(const char* name);

	ITexture* createEmptyTexture(const dimension2d& size, ECOLOR_FORMAT format);

private:
	void loadDefaultTextures();

private:
	ITexture*	DefaultWhite;

	std::map<std::string, ITexture*>	TextureMap;
	std::list<IRenderTarget*>	RenderTargets;
};