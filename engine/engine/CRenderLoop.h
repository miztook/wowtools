#pragma once

#include "ISceneNode.h"
#include "RenderStruct.h"
#include "CCanvas.h"
#include <vector>
#include <map>
#include <algorithm>

class CCamera;

class CRenderLoop
{
public:
	CRenderLoop();
	~CRenderLoop();

public:
	void addRenderUnit(SRenderUnit* unit);
	
	void doRenderLoopPrepass(const CCamera* cam);
	void doRenderLoopForward(const CCamera* cam);

	void clearRenderUnits();

	CCanvas* getCanvas() const { return m_Canvas.get(); }

private:
	void renderOpaques(const CCamera* cam);
	void renderShadowMap(const CCamera* cam);		//绘制shadowmap
	void collectShadows();			//收集阴影
	void renderImageEffects(const CCamera* cam);
	void renderAfterOpaues(const CCamera* cam);

	void processRenderUnit(SRenderUnit* unit);

private:
	//
	std::vector<const SRenderUnit*>		m_RenderUnits_Opaque;
	std::vector<const SRenderUnit*>		m_RenderUnits_AfterOpaque;

	//
	std::unique_ptr<CCanvas>		m_Canvas;
};