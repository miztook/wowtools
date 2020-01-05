#pragma once

#include "base.h"

class CLight
{
public:
	explicit CLight(E_LIGHT_TYPE type) : m_Type(type)
	{
		m_CullMask = 0xffffffff;
	}

public:
	void addLayer(int layer);
	void removeLayer(int layer);
	bool hasLayer(int layer) const;

private:
	E_LIGHT_TYPE	m_Type;
	uint32_t m_CullMask;
};