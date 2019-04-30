#pragma once

#include "predefine.h"

enum E_LIGHT_TYPE : int32_t
{
	ELT_POINT = 0,
	ELT_SPOT,
	ELT_DIRECTIONAL,
	ELT_AMBIENT,
};

enum E_VERTEX_TYPE : int32_t
{
	EVT_INVALID = -1,
	EVT_P = 0,
	EVT_PC,			//for bounding box
	EVT_PCT,
	EVT_PN,
	EVT_PNC,
	EVT_PNT,
	EVT_PT,
	EVT_PNCT,
	EVT_PNCT2,
	EVT_PNT2WA,						//fvf
	EVT_COUNT,
};

enum E_INDEX_TYPE : int
{
	EIT_INVALID = -1,
	EIT_16BIT = 0,
	EIT_32BIT,
	EIT_COUNT,
};
