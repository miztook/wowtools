#include "CSceneRenderer.h"
#include "CScene.h"
#include "Engine.h"

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
			scene->renderDebugInfo();

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
