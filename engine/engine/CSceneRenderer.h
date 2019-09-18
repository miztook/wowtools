#pragma once

#include "CTimer.h"
#include "CFPSCounter.h"
#include "SColor.h"
#include <vector>
#include "SCullResult.h"
#include "CRenderLoop.h"

class IVideoDriver;
class CScene;
class ISceneNode;

class CSceneRenderer
{
public:
	static CSceneRenderer* createSceneRenderer();

private:
	CSceneRenderer();

public:
	void renderFrame(const CScene* scene, bool active);
	float getFPS() const { return m_FPSCounter.getFPS(); }

private:
	void beginFrame();
	void endFrame();

	void render3D(const CScene* scene) const;
	void render2D(const CScene* scene) const;
	void renderDebugInfo() const;

private:
	CTimer		m_Timer;
	CFPSCounter	m_FPSCounter;
	IVideoDriver*	Driver;
	SColor	BackgroundColor;

	std::vector<ISceneNode*>	m_ProcessList;
	
	SCullResult		CullResult;
	CRenderLoop		RenderLoop;
};
