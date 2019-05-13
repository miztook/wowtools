#pragma once

#include "IVideoDriver.h"
#include "COpenGLExtension.h"

class COpenGLMaterialRenderComponent;
class COpenGLTextureWriteComponent;

class COpenGLDriver : public IVideoDriver
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGLDriver);

public:
	COpenGLDriver();
	virtual ~COpenGLDriver();

public:
	bool initDriver(const SWindowInfo& wndInfo, bool vsync, E_AA_MODE aaMode);

public:
	virtual bool beginScene();
	virtual bool endScene();
	virtual bool clear(bool renderTarget, bool zBuffer, bool stencil, SColor color);

	virtual bool checkValid();
	virtual bool setRenderTarget(const IRenderTarget* texture, bool bindDepth = true);

	virtual void setTransform(E_TRANSFORMATION_STATE state, const matrix4& mat);
	virtual void setTexture(int index, const ITexture* tex);

	virtual void setViewPort(const recti& area);
	virtual void setDisplayMode(const dimension2d& size);
	virtual bool setDriverSetting(const SDriverSetting& setting);

public:
	COpenGLMaterialRenderComponent* getMaterialRenderComponent() const { return MaterialRenderComponent.get();}
	COpenGLTextureWriteComponent* getTextureWriteComponent() const { return TextureWriteComponent.get(); }

private:
	dimension2d getWindowSize() const;
	void logCaps();
	bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const;

public:
	//device settings
	window_type		HWnd;
	dc_type					Hdc;
	glcontext_type		Hgrc;

	ECOLOR_FORMAT ColorFormat;
	ECOLOR_FORMAT DepthFormat;
	GLint		DefaultFrameBuffer;

	CAdapterInfo	AdapterInfo;
	COpenGLExtension	GLExtension;

private:
	E_RENDER_MODE		CurrentRenderMode;

	//component
	std::unique_ptr<COpenGLMaterialRenderComponent>  MaterialRenderComponent;
	std::unique_ptr<COpenGLTextureWriteComponent> TextureWriteComponent;
};