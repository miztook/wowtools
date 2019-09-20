#pragma once

#include <stdint.h>

enum E_SORTLAYER : uint16_t
{
	EST_DEFAULT = 0,
};

struct SGlobalLayerData
{
	SGlobalLayerData(): layer(0), order(0) {}

	uint16_t layer;
	uint16_t order;

	bool operator==(const SGlobalLayerData& rhs) const
	{
		return layer == rhs.layer && order == rhs.order;
	}

	bool operator!=(const SGlobalLayerData& rhs) const
	{
		return layer != rhs.layer || order != rhs.order;
	}

	bool operator<(const SGlobalLayerData& rhs) const
	{
		if (layer != rhs.layer)
			return layer < rhs.layer;
		else
			return order < rhs.order;
	}
};
