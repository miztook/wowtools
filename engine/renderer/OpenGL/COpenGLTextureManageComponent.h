#pragma once

#include "base.h"
#include <memory>
#include <map>
#include <list>
#include "vector2d.h"

class COpenGLDriver;
class ITexture;
class IRenderTarget;
class CCImage;

class COpenGLTextureManageComponent
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGLTextureManageComponent);

public:
	explicit COpenGLTextureManageComponent(const COpenGLDriver* driver);
	~COpenGLTextureManageComponent();

public:
	bool init();

	ITexture* getDefaultWhite() const { return DefaultWhite; }

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

	const COpenGLDriver*	Driver;
};
