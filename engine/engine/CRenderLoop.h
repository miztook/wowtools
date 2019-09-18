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
	
	void doRenderLoopPrepass();
	void doRenderLoopForward();

	void clearRenderUnits();

private:
	void renderOpaques();
	void renderAfterOpaues();

private:
	const SRenderUnit*			CurrentUnit;

	//
	std::vector<SRenderUnit>		m_RenderUnits_Opaque;
	std::vector<SRenderUnit>		m_RenderUnits_AfterOpaque;
};