#include "CLight.h"

void CLight::addLayer(int layer)
{
	m_CullMask |= (1 << layer);
}

void CLight::removeLayer(int layer)
{
	m_CullMask &= ~(1 << layer);
}

bool CLight::hasLayer(int layer) const
{
	return (m_CullMask & ~(1 << layer)) != 0;
}
