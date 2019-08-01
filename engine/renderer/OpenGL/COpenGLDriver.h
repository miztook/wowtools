#pragma once

#include "IVideoDriver.h"
#include "COpenGLExtension.h"
#include "RenderStruct.h"
#include <array>

class COpenGLMaterialRenderComponent;
class COpenGLTextureWriteComponent;
class COpenGLShaderManageComponent;
class COpenGLDrawHelperComponent;
class COpenGLTextureManageComponent;
class COpenGLVertexDeclaration;
class IVertexBuffer;
class IIndexBuffer;
class CGLProgram;

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

	void setTransform(E_TRANSFORMATION_STATE state, const matrix4& mat) override;
	void setTexture(int index, ITexture* tex) override;

	void setViewPort(const recti& area) override;
	void setDisplayMode(const dimension2d& size) override;
	bool setDriverSetting(const SDriverSetting& setting) override;

public:
	ITextureWriter* createTextureWriter(ITexture* texture) override;
	bool removeTextureWriter(ITexture* texture) override;

	ITexture* getTextureWhite() const override;
	void draw2DImageBatch(ITexture* texture,
		const vector2di positions[],
		const recti* sourceRects[],
		uint32_t batchCount,
		SColor color,
		E_RECT_UVCOORDS uvcoords,
		const S2DBlendParam& blendParam) override;

public:
	void draw(IVertexBuffer* vbuffer, IIndexBuffer* ibuffer,
		E_PRIMITIVE_TYPE primType,
		uint32_t primCount,
		const SDrawParam& drawParam,
		bool is2D);

public:
	COpenGLMaterialRenderComponent* getMaterialRenderComponent() const { return MaterialRenderComponent.get();}
	COpenGLTextureWriteComponent* getTextureWriteComponent() const { return TextureWriteComponent.get(); }
	COpenGLShaderManageComponent* getShaderManageComponent() const { return ShaderManageComponent.get(); }
	COpenGLDrawHelperComponent* getDrawHelperComponent() const { return DrawHelperComponent.get(); }

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

	matrix4		VP, T_VP;
	matrix4		W, T_W;
	matrix4		V, T_V;
	matrix4		P, T_P;
	matrix4		VP2D, T_VP2D;

private:
	//component
	std::unique_ptr<COpenGLMaterialRenderComponent>  MaterialRenderComponent;
	std::unique_ptr<COpenGLTextureWriteComponent> TextureWriteComponent;
	std::unique_ptr<COpenGLShaderManageComponent> ShaderManageComponent;
	std::unique_ptr<COpenGLDrawHelperComponent> DrawHelperComponent;
	std::unique_ptr<COpenGLTextureManageComponent>	TextureManageComponent;

	std::array<std::unique_ptr<COpenGLVertexDeclaration>, EVT_COUNT>		VertexDeclarations;

};