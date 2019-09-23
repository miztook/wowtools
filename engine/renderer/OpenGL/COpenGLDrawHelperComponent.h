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

		
		MAX_2DLINE_BATCH_COUNT = 512,
		MAX_3DLINE_BATCH_COUNT = 2048,
		MAX_IMAGE_BATCH_COUNT = MAX_QUADS,
		MAX_VERTEX_COUNT = MAX_VERTEXCOUNT,
		MAX_VERTEX_2D_COUNT = 64,
		MAX_INDEX_2D_COUNT = 128,
	};

public:
	bool init();

	IVertexBuffer* getStaticVertexBufferScreenQuad() const { return StaticVertexBufferScreenQuad.get(); }
	IVertexBuffer* getStaticVertexBufferScreenQuadFlip() const { return StaticVertexBufferScreenQuadFlip.get(); }

	IIndexBuffer* getStaticIndexBufferQuadList() const { return StaticIndexBufferQuadList.get(); }
	IIndexBuffer* getStaticIndexBufferTriangleList() const { return StaticIndexBufferTriangleList.get(); }

	//
	void add2DColor(const recti& rect, SColor color, E_2DBlendMode mode = E_Solid);
	void add2DQuads(ITexture* texture, const SVertex_PCT* vertices, uint32_t numQuads, const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	void flushAll2DQuads();


	//
	void draw2DImageBatch(ITexture* texture,
		const vector2di positions[],
		const recti* sourceRects[],
		uint32_t batchCount,
		SColor color,
		E_RECT_UVCOORDS uvcoords,
		const S2DBlendParam& blendParam);
	void draw2DSquadBatch(ITexture* texture,
		const SVertex_PCT* verts,
		uint32_t numQuads,
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
	void do_draw2DSquadBatch(uint32_t batchCount, ITexture* texture, const SVertex_PCT* vertices, uint32_t numQuads, const S2DBlendParam& blendParam);
	
private:
	struct SQuadDrawBatchKey
	{
		S2DBlendParam blendParam;
		ITexture* texture;

		SQuadDrawBatchKey(ITexture* tex, const S2DBlendParam& param)
			: blendParam(param), texture(tex)
		{
		}

		bool operator<(const SQuadDrawBatchKey& other) const
		{
			if (texture != other.texture)
				return texture < other.texture;
			else if (blendParam != other.blendParam)
				return blendParam < other.blendParam;
			else
				return false;
		}
	};

	struct SQuadBatchDraw
	{
		uint32_t vertNum() const { return (uint32_t)drawVerts.size(); }
		std::vector<SVertex_PCT>	drawVerts;
	};

	using T_QuadDrawMap = std::map<SQuadDrawBatchKey, SQuadBatchDraw>;

private:
	COpenGLDriver* Driver;

	std::unique_ptr<IVertexBuffer>			StaticVertexBufferScreenQuad;
	std::unique_ptr<IVertexBuffer>			StaticVertexBufferScreenQuadFlip;
	std::unique_ptr<IIndexBuffer>			StaticIndexBufferQuadList;
	std::unique_ptr<IIndexBuffer>			StaticIndexBufferTriangleList;

	SVertex_PCT*		ImageVertices;

	std::unique_ptr<IVertexBuffer>		VBImage;

	SMaterial	InitMaterial2D;

	//
	T_QuadDrawMap		m_2DQuadDrawMap;
	T_QuadDrawMap		m_3DQuadDrawMap;
};