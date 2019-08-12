#pragma once

#include "CTimer.h"
#include "CFPSCounter.h"
#include "SColor.h"

class IVideoDriver;
class CScene;

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

	void renderDebugInfo() const;

private:
	CTimer		m_Timer;
	CFPSCounter	m_FPSCounter;
	IVideoDriver*	Driver;
	SColor	BackgroundColor;
};
