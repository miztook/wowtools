#pragma once

#include "IVideoDriver.h"
#include "COpenGLExtension.h"
#include <array>

class COpenGLMaterialRenderComponent;
class COpenGLTextureWriteComponent;
class COpenGLShaderManageComponent;
class COpenGLTextureManageComponent;
class COpenGLVertexDeclaration;
class IVertexBuffer;
class IIndexBuffer;
class CGLProgram;
struct SDrawParam;

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
	bool beginScene() override;
	bool endScene() override;
	bool clear(bool renderTarget, bool zBuffer, bool stencil, SColor color) override;

	bool checkValid() override { return true; }
	bool setRenderTarget(const IRenderTarget* texture, bool bindDepth = true) override;

	void setViewPort(const recti& area) override;
	void setDisplayMode(const dimension2d& size) override;
	bool setDriverSetting(const SDriverSetting& setting) override;

	void draw(const CPass* pass, const IVertexBuffer* vbuffer, const IIndexBuffer* ibuffer,
		E_PRIMITIVE_TYPE primType,
		uint32_t primCount,
		const SDrawParam& drawParam) override;

public:
	IVertexBuffer* createVertexBuffer(E_MESHBUFFER_MAPPING mapping) override;
	IIndexBuffer* createIndexBuffer(E_MESHBUFFER_MAPPING mapping) override;

	ITextureWriter* createTextureWriter(ITexture* texture) override;
	bool removeTextureWriter(ITexture* texture) override;

	ITexture* createEmptyTexture(const dimension2d& size, ECOLOR_FORMAT format) override;
	ITexture* getTextureWhite() const override;
	
public:
	COpenGLMaterialRenderComponent* getMaterialRenderComponent() const { return MaterialRenderComponent.get();}
	COpenGLTextureWriteComponent* getTextureWriteComponent() const { return TextureWriteComponent.get(); }
	COpenGLShaderManageComponent* getShaderManageComponent() const { return ShaderManageComponent.get(); }

	void deleteVao(const IVertexBuffer* vbuffer);

private:
	dimension2d getWindowSize() const;
	void logCaps();
	bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const;
	bool reset();

	//
	COpenGLVertexDeclaration* getVertexDeclaration(E_VERTEX_TYPE type) const { return VertexDeclarations[type].get(); }
	void setVertexDeclarationAndBuffers(const CGLProgram* program, const IVertexBuffer* vbuffer0, uint32_t offset0, const IIndexBuffer* ibuffer);
	void drawIndexedPrimitive(const IVertexBuffer* vbuffer, const IIndexBuffer* ibuffer, const CGLProgram* program,
		E_PRIMITIVE_TYPE primType,
		uint32_t primCount,
		const SDrawParam& drawParam);

	//
	void createResources(const dimension2d& windowSize);
	void destroyResources();

public:
	//device settings
	window_type		HWnd;
	dc_type					Hdc;
	glcontext_type		Hgrc;

	ECOLOR_FORMAT ColorFormat;
	ECOLOR_FORMAT DepthFormat;
	GLint		DefaultFrameBuffer;

	COpenGLExtension	GLExtension;

private:
	//component
	std::unique_ptr<COpenGLMaterialRenderComponent>  MaterialRenderComponent;
	std::unique_ptr<COpenGLTextureWriteComponent> TextureWriteComponent;
	std::unique_ptr<COpenGLShaderManageComponent> ShaderManageComponent;
	std::unique_ptr<COpenGLTextureManageComponent>	TextureManageComponent;

	std::array<std::unique_ptr<COpenGLVertexDeclaration>, EVT_COUNT>		VertexDeclarations;

};