#include "CSceneRenderer.h"
#include "CScene.h"
#include "Engine.h"
#include "EngineUtil.h"
#include "CCamera.h"

CSceneRenderer * CSceneRenderer::createSceneRenderer()
{
	return new CSceneRenderer;
}

CSceneRenderer::CSceneRenderer()
	: BackgroundColor(64, 64, 64)
{
	Driver = g_Engine->getDriver();
}

void CSceneRenderer::renderFrame(const CScene* scene, bool active)
{
	beginFrame();


	if (Driver->checkValid())
	{
		Driver->setRenderTarget(nullptr);

		if (Driver->beginScene())
		{
			Driver->clear(true, true, false, BackgroundColor);

			scene->render3D();
			scene->render2D();

			if (scene->get2DCamera()->IsInited())
				renderDebugInfo();

			Driver->endScene();
		}
	}

	endFrame();
}

void CSceneRenderer::beginFrame()
{
	m_Timer.calculateTime();
}

void CSceneRenderer::endFrame()
{
	m_FPSCounter.registerFrame(CSysChrono::getTimePointNow());
}

void CSceneRenderer::renderDebugInfo() const
{
	char debugMsg[512];
	const IVideoDriver* driver = g_Engine->getDriver();
	Q_sprintf(debugMsg, 512, "Dev: %s\nGraphics: %s\nRes: %d X %d\nFPS: %0.1f\nTriangles: %u\nDraw Call: %u\n",
		driver->AdapterInfo.description.c_str(),
		driver->AdapterInfo.name.c_str(),
		driver->getViewPort().getWidth(),
		driver->getViewPort().getHeight(),
		m_FPSCounter.getFPS(),
		driver->PrimitivesDrawn,
		driver->DrawCall);
	EngineUtil::drawDebugInfo(debugMsg);
}
