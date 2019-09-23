#pragma once

#include "base.h"
#include "matrix4.h"
#include "SMaterial.h"
#include "CShaderUtil.h"

class IVertexBuffer;
class IIndexBuffer;
class IRenderer;

enum E_2DBlendMode : int8_t
{
	E_Solid = 0,
	E_AlphaBlend,
	E_OneAlpha,
	E_AddAlpha,
	E_AddColor,
	E_Modulate,
	E_ModulateX2,
	E_OneOne,
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

	E_BLEND_TYPE getBlendType() const
	{
		if (!alpha && !alphaChannel)
			return EMT_SOLID;

		switch (blendMode)
		{
		case E_AlphaBlend:
			return EMT_TRANSPARENT_ALPHA_BLEND;
		case E_OneAlpha:
			return EMT_TRANSPARENT_ONE_ALPHA;
		case E_AddAlpha:
			return EMT_TRANSPARENT_ADD_ALPHA;
		case E_AddColor:
			return EMT_TRANSPARENT_ADD_COLOR;
		case E_Modulate:
			return EMT_TRANSPARENT_MODULATE;
		case E_ModulateX2:
			return EMT_TRANSPARENT_MODULATE_X2;
		case E_OneOne:
			return EMT_TRANSPARENT_ONE_ONE;
		default:
			return EMT_SOLID;
		}
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

	void setMaterial(SMaterial& material) const
	{
		material.VSFile = "";
		material.BlendType = getBlendType();
		material.PSFile = "UI";
		material.PSMacroString = CShaderUtil::getUIPSMacroString(alpha, alphaChannel);
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