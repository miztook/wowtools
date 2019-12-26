#pragma once

#include "CTimer.h"
#include "CFPSCounter.h"
#include "SColor.h"
#include <vector>
#include <map>
#include "SCullResult.h"
#include "CRenderLoop.h"

class CScene;
class ISceneNode;
class CCamera;

class CSceneRenderer
{
public:
	static CSceneRenderer* createSceneRenderer();
	~CSceneRenderer();

private:
	CSceneRenderer();

public:
	void renderFrame(const CScene* scene, bool active);
	float getFPS() const { return m_FPSCounter.getFPS(); }

private:
	struct SCameraRender
	{
		explicit SCameraRender(const CCamera* cam) : Camera(cam) {}

		const CCamera*	Camera;
		SColor		BackgroundColor;
		std::vector<ISceneNode*>	SceneNodeList;			//camera处理的scene node
		SCullResult		CullResult;						//cull的IRenderer
		CRenderLoop		RenderLoop;					//RenderLoop
	};

private:
	void beginFrame();
	void endFrame();

	void renderDebugInfo() const;
	SCameraRender* getCameraRender(const CCamera* cam);

private:
	CTimer		m_Timer;
	CFPSCounter	m_FPSCounter;
	
	std::map<const CCamera*, SCameraRender*>		CameraRenderMap;
};
