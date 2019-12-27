#pragma once

#include "base.h"
#include "matrix4.h"

class IVertexBuffer;
class IIndexBuffer;
class IRenderer;

enum E_2DBlendMode : int8_t
{
	E_Solid = 0,
	E_AlphaBlend,
	E_OneAlpha,
};

struct S2DBlendParam
{
	E_2DBlendMode blendMode;
	bool alpha;
	bool alphaChannel;

	S2DBlendParam()
		: alpha(false), alphaChannel(false), blendMode(E_Solid)
	{
	}

	S2DBlendParam(bool _alpha, bool _alphaChannel, E_2DBlendMode _blendMode)
		: alpha(_alpha), alphaChannel(_alphaChannel), blendMode(_blendMode)
	{
	}

	int getHashValue() const
	{
		return ((blendMode << 8) + ((alpha ? 1 : 0) << 4) + (alphaChannel ? 1 : 0));
	}

	bool operator!=(const S2DBlendParam& b) const
	{
		return alpha != b.alpha ||
			alphaChannel != b.alphaChannel ||
			blendMode != b.blendMode;
	}

	bool operator==(const S2DBlendParam& b) const
	{
		return alpha == b.alpha &&
			alphaChannel == b.alphaChannel &&
			blendMode == b.blendMode;
	}

	bool operator<(const S2DBlendParam& c) const
	{
		int a = ((blendMode << 8) + ((alpha ? 1 : 0) << 4) + (alphaChannel ? 1 : 0));
		int b = ((c.blendMode << 8) + ((c.alpha ? 1 : 0) << 4) + (c.alphaChannel ? 1 : 0));
		return a < b;
	}

	static const S2DBlendParam& OpaqueSource()
	{
		static S2DBlendParam m(false, false, E_Solid);
		return m;
	}

	static const S2DBlendParam& UIFontBlendParam()
	{
		static S2DBlendParam m(false, true, E_OneAlpha);
		return m;
	}

	static const S2DBlendParam& UITextureBlendParam()
	{
		static S2DBlendParam m(false, true, E_AlphaBlend);
		return m;
	}
};

struct  SDrawParam
{
	SDrawParam() : voffset(0), baseVertIndex(0), minVertIndex(0), numVertices(0), startIndex(0) {}

	uint32_t		voffset;
	int		baseVertIndex;
	uint32_t		minVertIndex;
	uint32_t		numVertices;
	uint32_t		startIndex;
};

struct SRenderUnit
{
	explicit SRenderUnit(const IRenderer* rdr) : renderer(rdr)
	{
		vbuffer = nullptr;
		ibuffer = nullptr;
		primCount = 0;
		primType = EPT_TRIANGLES;
		distance = 0;
		shaderSortId = 0;
	}

	const IRenderer* renderer;
	IVertexBuffer* vbuffer;
	IIndexBuffer* ibuffer;
	SDrawParam  drawParam;
	uint32_t		primCount;
	E_PRIMITIVE_TYPE	primType;
	float distance;
	int shaderSortId;

};