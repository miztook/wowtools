#include "CRenderLoop.h"
#include "SMaterial.h"

bool SolidCompare(const SRenderUnit& a, const SRenderUnit& b)
{
	return true;
}

bool AlphaTestCompare(const SRenderUnit& a, const SRenderUnit& b)
{
	return true;
}

bool TransparentCompare(const SRenderUnit& a, const SRenderUnit& b)
{
	return true;
}

bool OverlayCompare(const SRenderUnit& a, const SRenderUnit& b)
{
	return true;
}

RENDERUNIT_COMPARE_FUNC getCompareFunc(int renderQueue)
{
	if (renderQueue < ERQ_ALPHATEST)		//solid
	{
		return SolidCompare;
	}
	else if (renderQueue < ERQ_TRANSPARENT)			//alphatest
	{
		return AlphaTestCompare;
	}
	else if (renderQueue < ERQ_OVERLAY)		//transparent
	{
		return TransparentCompare;
	}
	else
	{
		return OverlayCompare;
	}
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

	if (renderQueue <= ERQ_BACKGROUND)
		m_RenderUnits_Background.push_back(unit);
	else if (renderQueue <= ERQ_GEOMETRY)
		m_RenderUnits_Geometry.push_back(unit);
	else if (renderQueue <= ERQ_ALPHATEST)
		m_RenderUnits_AlphaTest.push_back(unit);
	else if (renderQueue <= ERQ_GEOMETRYLAST)
		m_RenderUnits_GeometryLast.push_back(unit);
	else if (renderQueue <= ERQ_TRANSPARENT)
		m_RenderUnits_Transparent.push_back(unit);
	else if (renderQueue <= ERQ_OVERLAY)
		m_RenderUnits_Overlay.push_back(unit);
}

void CRenderLoop::renderAll()
{

}

