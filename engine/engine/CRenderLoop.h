#pragma once

#include "ISceneNode.h"
#include "RenderStruct.h"
#include <vector>
#include <map>
#include <algorithm>

using RENDERUNIT_COMPARE_FUNC = bool(*)(const SRenderUnit& a, const SRenderUnit& b);
using SCENENODE_COMPARE_FUNC = bool(*)(const ISceneNode* a, const ISceneNode* b);

class CRenderLoop
{
public:
	CRenderLoop();
	~CRenderLoop();

public:
	void addRenderUnit(const SRenderUnit& unit);
	void renderAll();

private:
	const SRenderUnit*			CurrentUnit;

	//
	std::vector<SRenderUnit>		m_RenderUnits_Background;
	std::vector<SRenderUnit>		m_RenderUnits_Geometry;
	std::vector<SRenderUnit>		m_RenderUnits_AlphaTest;
	std::vector<SRenderUnit>		m_RenderUnits_GeometryLast;
	std::vector<SRenderUnit>		m_RenderUnits_Transparent;
	std::vector<SRenderUnit>		m_RenderUnits_Overlay;
};