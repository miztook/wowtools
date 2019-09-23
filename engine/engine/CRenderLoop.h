#pragma once

#include "ISceneNode.h"
#include "RenderStruct.h"
#include <vector>
#include <map>
#include <algorithm>

class IVideoDriver;

class CRenderLoop
{
public:
	CRenderLoop();
	~CRenderLoop();

public:
	void addRenderUnit(const SRenderUnit* unit);
	
	void doRenderLoopPrepass(const CCamera* cam);
	void doRenderLoopForward(const CCamera* cam);

	void clearRenderUnits();

private:
	void renderOpaques(const CCamera* cam);
	void renderAfterOpaues(const CCamera* cam);

private:
	IVideoDriver*	Driver;
	const SRenderUnit*			CurrentUnit;

	//
	std::vector<const SRenderUnit*>		m_RenderUnits_Opaque;
	std::vector<const SRenderUnit*>		m_RenderUnits_AfterOpaque;
};