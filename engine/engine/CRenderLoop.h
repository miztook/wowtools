#pragma once

#include "ISceneNode.h"
#include "RenderStruct.h"
#include <vector>
#include <map>
#include <algorithm>

class CRenderLoop
{
public:
	CRenderLoop();
	~CRenderLoop();

public:
	void addRenderUnit(const SRenderUnit* unit);
	
	void doRenderLoopPrepass();
	void doRenderLoopForward();

	void clearRenderUnits();

private:
	void renderOpaques();
	void renderAfterOpaues();

private:
	const SRenderUnit*			CurrentUnit;

	//
	std::vector<const SRenderUnit*>		m_RenderUnits_Opaque;
	std::vector<const SRenderUnit*>		m_RenderUnits_AfterOpaque;
};