#include "CRenderLoop.h"
#include "SMaterial.h"

bool OpaqueCompare(const SRenderUnit& a, const SRenderUnit& b)
{
	return true;
}

bool AfterOpaqueCompare(const SRenderUnit& a, const SRenderUnit& b)
{
	return true;
}

RENDERUNIT_COMPARE_FUNC getCompareFunc(int renderQueue)
{
	if (renderQueue <= ERQ_GEOMETRY_INDEX_MAX)		//solid
	{
		return OpaqueCompare;
	}
	else if (renderQueue <= ERQ_INDEX_MAX)		//transparent
	{
		return AfterOpaqueCompare;
	}
	return nullptr;
}

CRenderLoop::CRenderLoop()
{

}

CRenderLoop::~CRenderLoop()
{

}

void CRenderLoop::addRenderUnit(const SRenderUnit& unit)
{
	int renderQueue = unit.material->RenderQueue;

	if (renderQueue <= ERQ_GEOMETRY_INDEX_MAX)		//solid
	{
		m_RenderUnits_Opaque.push_back(unit);
	}
	else if (renderQueue <= ERQ_INDEX_MAX)		//transparent
	{
		m_RenderUnits_AfterOpaque.push_back(unit);
	}
}

void CRenderLoop::doRenderLoopPrepass()
{

}

void CRenderLoop::doRenderLoopForward()
{
	renderOpaques();
	renderAfterOpaues();
}

void CRenderLoop::clearRenderUnits()
{
	m_RenderUnits_Opaque.clear();
	m_RenderUnits_AfterOpaque.clear();
}

void CRenderLoop::renderOpaques()
{
	std::sort(m_RenderUnits_Opaque.begin(), m_RenderUnits_Opaque.end(), OpaqueCompare);


}

void CRenderLoop::renderAfterOpaues()
{
	std::sort(m_RenderUnits_AfterOpaque.begin(), m_RenderUnits_AfterOpaque.end(), AfterOpaqueCompare);
}

