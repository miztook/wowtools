#include "CSceneRenderer.h"
#include "CScene.h"
#include "Engine.h"
#include "EngineUtil.h"
#include "CCamera.h"
#include "ISceneNode.h"
#include "IRenderer.h"
#include "CRenderLoop.h"
#include <algorithm>

bool SceneNodeCompare(const ISceneNode* a, const ISceneNode* b)
{
	uint32_t h1 = a->getTransform()->getHiearchyCount();
	uint32_t h2 = b->getTransform()->getHiearchyCount();

	if (h1 != h2)
		return h1 < h2;
	else
		return a < b;
}

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

	uint32_t tickTime = m_Timer.getTimeSinceLastFrame();

	//collect scene node
	m_ProcessList.clear();
	const std::list<ISceneNode*>& sceneNodeList = scene->getTopSceneNodeList();
	for (ISceneNode* node : sceneNodeList)
	{
		node->traverse([this](ISceneNode* n) 
		{ 
			if (n->activeSelf() && !n->isToDelete())
				m_ProcessList.push_back(n); 
		});
	}

	//scene node tick
	std::sort(m_ProcessList.begin(), m_ProcessList.end(), SceneNodeCompare);
	for (const auto& node : m_ProcessList)
	{
		node->tick(tickTime);
	}

	//3d camera
	const CCamera* cam = scene->get3DCamera();
	if (cam->IsInited())
	{
		//cull renderers
		CullResult.VisibleRenderers.clear();
		for (const auto& node : m_ProcessList)
		{
			const std::list<IRenderer*> rendererList = node->getRendererList();
			for (const IRenderer* renderer : rendererList)
			{
				const aabbox3df& box = renderer->getBoundingBox();
				if (cam->getWorldFrustum().isInFrustum(box))
					CullResult.VisibleRenderers.push_back(renderer);
			}
		}

		//scene node renderer
		for (const IRenderer* renderer : CullResult.VisibleRenderers)
		{
			ISceneNode* node = renderer->getSceneNode();
			SRenderUnit* renderUnit = node->render(renderer, cam);
			if (renderUnit)
				RenderLoop.addRenderUnit(renderUnit);
		}
	}
	
	//actual render
	if (Driver->checkValid())
	{
		Driver->setRenderTarget(nullptr);

		if (Driver->beginScene())
		{
			Driver->clear(true, true, false, BackgroundColor);

			RenderLoop.doRenderLoopPrepass();
			RenderLoop.doRenderLoopForward();

			renderDebugInfo();

			Driver->endScene();
		}
	}
	RenderLoop.clearRenderUnits();

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

void CSceneRenderer::render2D(const CScene* scene) const
{
	if (!scene->get2DCamera()->IsInited())
		return;
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
