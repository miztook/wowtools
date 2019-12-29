#pragma once

#include <stdint.h>
#include "base.h"
#include "SColor.h"
#include "vector3d.h"
#include "vector2d.h"

struct SVertex_PC
{
	vector3df	Pos;
	SColor		Color;

	void set(const vector3df& p, SColor c) { Pos = p; Color = c; }

	static E_VERTEX_TYPE TYPE() { return EVT_PC; }
};

struct SVertex_PCT
{
	vector3df Pos;
	SColor	Color;
	vector2df TCoords;

	void set(const vector3df& p, SColor c, const vector2df& t) { Pos = p; Color = c; TCoords = t; }

	static E_VERTEX_TYPE TYPE() { return EVT_PCT; }
};

struct SVertex_PNC
{
	vector3df	Pos;
	vector3df Normal;
	SColor Color;

	void set(const vector3df& p, const vector3df& n, SColor c) { Pos = p; Normal = n; Color = c; }

	static E_VERTEX_TYPE TYPE() { return EVT_PNC; }
};

struct SVertex_PNCT
{
	vector3df	Pos;
	vector3df Normal;
	SColor Color;
	vector2df TCoords;

	void set(const vector3df& p, const vector3df& n, SColor c, const vector2df& t) { Pos = p; Normal = n; Color = c; TCoords = t; }

	static E_VERTEX_TYPE TYPE() { return EVT_PNCT; }
};

struct SVertex_PNCT2
{
	vector3df	Pos;
	vector3df Normal;
	SColor Color;
	vector2df TCoords0;
	vector2df TCoords1;

	void set(const vector3df& p, const vector3df& n, SColor c, const vector2df& t0, const vector2df& t1) { Pos = p; Normal = n; Color = c; TCoords0 = t0; TCoords1 = t1; }

	static E_VERTEX_TYPE TYPE() { return EVT_PNCT2; }
};

struct SVertex_PNT2WA
{
	vector3df Pos;
	vector3df Normal;
	vector2df TCoords0;
	vector2df TCoords1;
	uint8_t		Weights[4];
	uint8_t		BoneIndices[4];

	void set(const vector3df& p, const vector3df& n, const vector2df& t, const vector2df& t1)
	{
		Pos = p; Normal = n; TCoords0 = t; TCoords1 = t1;
	}

	static E_VERTEX_TYPE TYPE() { return EVT_PNT2WA; }
};

inline uint32_t getVertexPitchFromType(E_VERTEX_TYPE type)
{
	switch (type)
	{
	case EVT_PC:
		return sizeof(SVertex_PC);
	case EVT_PCT:
		return sizeof(SVertex_PCT);
	case EVT_PNC:
		return sizeof(SVertex_PNC);
	case EVT_PNCT:
		return sizeof(SVertex_PNCT);
	case EVT_PNCT2:
		return sizeof(SVertex_PNCT2);
	case EVT_PNT2WA:
		return sizeof(SVertex_PNT2WA);

	default:
		ASSERT(false);
		return 0;
	}
}

inline bool vertexHasColor(E_VERTEX_TYPE type)
{
	return type == EVT_PC || type == EVT_PCT || type == EVT_PNC || type == EVT_PNCT || type == EVT_PNCT2;
}

inline void deleteVerticesFromType(E_VERTEX_TYPE type, void* vertices)
{
	switch (type)
	{
	case EVT_PC:
		DELETE_ARRAY(SVertex_PC, vertices);
		break;
	case EVT_PCT:
		DELETE_ARRAY(SVertex_PCT, vertices);
		break;
	case EVT_PNC:
		DELETE_ARRAY(SVertex_PNC, vertices);
		break;
	case EVT_PNCT:
		DELETE_ARRAY(SVertex_PNCT, vertices);
		break;
	case EVT_PNCT2:
		DELETE_ARRAY(SVertex_PNCT2, vertices);
		break;
	case EVT_PNT2WA:
		DELETE_ARRAY(SVertex_PNT2WA, vertices);
		break;
	default:
		ASSERT(false);
		break;
	}
}

inline void deleteIndicesFromType(E_INDEX_TYPE type, void* indices)
{
	switch (type)
	{
	case EIT_16BIT:
		DELETE_ARRAY(uint16_t, indices);
		break;
	case EIT_32BIT:
		DELETE_ARRAY(uint32_t, indices);
		break;
	default:
		ASSERT(false);
		break;
	}
}