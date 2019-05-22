#pragma once

#include "base.h"
#include "matrix4.h"

class IVertexBuffer;
class IIndexBuffer;
struct SMaterial;

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

struct  SDrawParam
{
	uint32_t		voffset0;
	int		baseVertIndex;
	uint32_t		minVertIndex;
	uint32_t		numVertices;
	uint32_t		startIndex;
};

struct SRenderUnit
{
	float distance;
	const SMaterial* material;
	IVertexBuffer* vbuffer;
	IIndexBuffer* ibuffer;
	SDrawParam  drawParam;
	matrix4		matWorld;
	uint32_t		primCount;
	E_PRIMITIVE_TYPE	primType;
};