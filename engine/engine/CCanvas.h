#pragma once

#include "base.h"
#include "RenderStruct.h"
#include "vector2d.h"
#include "rect.h"
#include "SColor.h"
#include "S3DVertex.h"
#include "CMaterial.h"
#include <map>

class CCamera;

class CCanvas
{
public:
	CCanvas();
	~CCanvas();

public:
	//
	void add2DColor(const recti&rect, SColor color, E_2DBlendMode mode = E_Solid);
	void add2DQuads(ITexture* texture, const SVertex_PCT* vertices, uint32_t numQuads, const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	void renderSubBatch(const CCamera* cam);

private:
	void draw2DSquadBatch(const CCamera* cam, ITexture* texture,
		const SVertex_PCT* verts,
		uint32_t numQuads,
		const S2DBlendParam& blendParam);

	rectf setUVCoords(E_RECT_UVCOORDS uvcoords, float x0, float y0, float x1, float y1);

	void draw2DSquad(const CCamera* cam, uint32_t batchCount, ITexture* texture, const SVertex_PCT* vertices, uint32_t numQuads, const S2DBlendParam& blendParam);

	void applyBlendParam(const S2DBlendParam& blendParam, CPass* pass);

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

private:
	CMaterial	Material;

	std::map<SQuadDrawBatchKey, SQuadBatchDraw>		m_2DQuadDrawMap;
};