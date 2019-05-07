#pragma once

#include "base.h"
#include <string>
#include <list>

enum E_RENDER_MODE : int32_t
{
	ERM_NONE = 0,
	ERM_2D,
	ERM_3D,
	ERM_STENCIL_FILL,
};

struct SDriverSetting
{
	SDriverSetting()
	{
		vsync = true;
		aaMode = E_AA_MSAA_1;
	}

	bool vsync;
	E_AA_MODE aaMode;
};

struct SDisplayMode
{
	SDisplayMode()
	{
		width = 0;
		height = 0;
		refreshrate = 60;
	}

	uint16_t width;
	uint16_t height;
	uint32_t refreshrate;

	bool operator==(const SDisplayMode& mode) const
	{
		return width == mode.width &&
			height == mode.height &&
			refreshrate == mode.refreshrate;
	}
};

class CAdapterInfo
{
public:
	CAdapterInfo() : index(0), vendorID(0)
	{
	}

public:
	uint32_t		index;
	uint32_t		vendorID;
	std::string		description;
	std::string		name;
	std::string		vendorName;

	typedef std::list<SDisplayMode>	T_DisplayModeList;
	T_DisplayModeList displayModes;

	void addDisplayMode(const SDisplayMode& mode)
	{
		if (displayModes.end() == std::find(displayModes.begin(), displayModes.end(), mode))
		{
			displayModes.push_back(mode);
		}
	}

	SDisplayMode getCloseMatchDisplayMode(uint32_t width, uint32_t height) const
	{
		SDisplayMode ret;
		//select first
		T_DisplayModeList::const_iterator itr = displayModes.begin();
		uint32_t diff = abs(((int)itr->width - (int)width)) + abs(((int)itr->height - (int)height));
		ret = (*itr);

		++itr;
		for (; itr != displayModes.end(); ++itr)
		{
			uint32_t d = abs(((int)itr->width - (int)width)) + abs(((int)itr->height - (int)height));
			if (d < diff)
			{
				diff = d;
				ret = (*itr);
			}
		}

		return ret;
	}
};
