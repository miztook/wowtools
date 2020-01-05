#include "CLightSetting.h"

CLightSetting::CLightSetting()
{

}

bool CLightSetting::hasLight(int layer) const
{
	for (const auto& light : m_Lights)
	{
		if (light.hasLayer(layer))
			return true;
	}
	return false;
}
