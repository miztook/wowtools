#pragma once

#include "CLight.h"
#include <list>

class CLightSetting
{
public:
	CLightSetting();

public:
	bool hasLight(int layer) const;

private:
	std::list<CLight>	m_Lights;
};