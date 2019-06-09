#pragma once

#include "base.h"
#include "RenderStruct.h"
#include "vector2d.h"
#include "rect.h"
#include "SColor.h"
#include "S3DVertex.h"
#include <vector>

class COpenGLDriver;
class ITexture;
class IVertexBuffer;
class IIndexBuffer;

class COpenGLDrawHelperComponent
{
public:
	explicit COpenGLDrawHelperComponent(COpenGLDriver* driver);
	~COpenGLDrawHelperComponent();

	enum EConstant : uint32_t
	{
		MAX_QUADS = 128,
		MAX_VERTEXCOUNT = 1024 * 3,			//dynamic vertex count
	};

public:
	bool init();

	IVertexBuffer* getStaticVertexBufferScreenQuad() const { return StaticVertexBufferScreenQuad.get(); }
	IVertexBuffer* getStaticVertexBufferScreenQuadFlip() const { return StaticVertexBufferScreenQuadFlip.get(); }

	IIndexBuffer* getStaticIndexBufferQuadList() const { return StaticIndexBufferQuadList.get(); }
	IIndexBuffer* getStaticIndexBufferTriangleList() const { return StaticIndexBufferTriangleList.get(); }

	//
	void draw2DImageBatch(ITexture* texture,
		const vector2di positions[],
		const recti* sourceRects[],
		uint32_t batchCount,
		SColor color,
		E_RECT_UVCOORDS uvcoords,
		const S2DBlendParam& blendParam);

private:
	void initMaterials();

	void createStaticVertexBufferScreenQuad();
	void destroyStaticVertexBufferScreenQuad();

	void createStaticIndexBufferQuadList();
	void destroyStaticIndexBufferQuadList();

	void createStaticIndexBufferTriangleList();
	void destroyStaticIndexBufferTriangleList();

	void createDynamicVertexBuffers();
	void destroyDynamicVertexBuffers();

	rectf setUVCoords(E_RECT_UVCOORDS uvcoords, float x0, float y0, float x1, float y1);
	void do_draw2DImageBatch(uint32_t batchCount, uint32_t nOffset, const vector2di* positions, const recti** sourceRects, ITexture* texture, E_RECT_UVCOORDS uvcoords, SColor color, const S2DBlendParam& blendParam);

private:
	COpenGLDriver* Driver;

	std::unique_ptr<IVertexBuffer>			StaticVertexBufferScreenQuad;
	std::unique_ptr<IVertexBuffer>			StaticVertexBufferScreenQuadFlip;
	std::unique_ptr<IIndexBuffer>			StaticIndexBufferQuadList;
	std::unique_ptr<IIndexBuffer>			StaticIndexBufferTriangleList;

	SVertex_PCT*		ImageVertices;

	std::unique_ptr<IVertexBuffer>		VBImage;

	SMaterial	InitMaterial2D;
	SGlobalMaterial		InitGlobalMaterial2D;
};