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