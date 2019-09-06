#include "CFrameRenderer.h"
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

CRenderUnitQueue::COMPARE_FUNC CRenderUnitQueue::getCompareFunc(int renderQueue)
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

CFrameRenderer::CFrameRenderer()
{

}

CFrameRenderer::~CFrameRenderer()
{

}

void CFrameRenderer::addSceneNode(ISceneNode* node)
{
	SEntry entry;
	entry.node = node;
	entry.skip = false;
	m_SceneNodes.emplace_back(entry);
}

void CFrameRenderer::skipSceneNode(ISceneNode* node)
{
	for (auto& entry : m_SceneNodes)
	{
		if (entry.node == node)
		{
			entry.skip = true;
			break;
		}
	}
}

void CFrameRenderer::clearAllSceneNodes()
{
	m_SceneNodes.clear();
}

void CFrameRenderer::tickAllSceneNodes(uint32_t tickTime)
{
	std::sort(m_SceneNodes.begin(), m_SceneNodes.end());
	for (const auto& entry : m_SceneNodes)
	{
		if (entry.skip)
			continue;
		ISceneNode* node = entry.node;
		node->tick(tickTime);
	}
}

void CFrameRenderer::addRenderUnit(const SRenderUnit* unit)
{

}
