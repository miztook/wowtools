#pragma once

#include "function.h"

enum class WowDBType : int
{
	Unknown = 0,
	WDBC,
	WDB2,
	WDB5,
	WDB6,
	WDC1,
	WDC2,
	WDC3,
};

enum M2Type : int32_t
{
	MT_NONE = 0,
	MT_CHARACTER,
	MT_CREATRUE,
	MT_ITEM,
	MT_SPELLS,
	MT_PARTICLES,
	MT_INTERFACE,
	MT_WORLD,
	MT_COUNT,
};

static const char* M2TypeString[] =
{
	"",
	"character",
	"creature",
	"item",
	"spells",
	"particles",
	"interface",
	"world",
	"",
};

inline M2Type getM2Type(const char* dir)
{
	for (uint32_t i = 1; i < MT_COUNT; ++i)
	{
		if (0 == Q_strnicmp(M2TypeString[i], dir, strlen(M2TypeString[i])))
			return (M2Type)i;
	}
	return MT_NONE;
}

//texture
//人物的特殊贴图指可以变化的贴图，如头发，肤色等
enum ETextureTypes : int
{
	TEXTURE_FILENAME = 0,			// Texture given in filename
	TEXTURE_BODY,				// Body + clothes
	TEXTURE_CAPE,				// Item, Capes ("Item\ObjectComponents\Cape\*.blp")
	TEXTURE_ITEM = TEXTURE_CAPE,
	TEXTURE_ARMORREFLECT,		// 
	TEXTURE_HAIR = 6,				// Hair, bear
	TEXTURE_FUR = 8,				// Tauren fur
	TEXTURE_INVENTORY_ART1,		// Used on inventory art M2s (1): inventoryartgeometry.m2 and inventoryartgeometryold.m2
	TEXTURE_QUILL,				// Only used in quillboarpinata.m2. I can't even find something referencing that file. Oo Is it used?
	TEXTURE_GAMEOBJECT1,		// Skin for creatures or gameobjects #1
	TEXTURE_GAMEOBJECT2,		// Skin for creatures or gameobjects #2
	TEXTURE_GAMEOBJECT3,		// Skin for creatures or gameobjects #3
	TEXTURE_INVENTORY_ART2,		// Used on inventory art M2s (2): ui-buffon.m2 and forcedbackpackitem.m2 (LUA::Model:ReplaceIconTexture("texture"))
	TEXTURE_15,					// Patch 12857, Unknown
	TEXTURE_16,					//
	TEXTURE_17,					//
	NUM_TEXTURETYPE,
};

enum TEXTUREFLAG : int32_t
{
	TEXTURE_WRAPX = 1,
	TEXTURE_WRAPY = 2,
};