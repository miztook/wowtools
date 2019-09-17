#include "CSceneRenderer.h"
#include "CScene.h"
#include "Engine.h"
#include "EngineUtil.h"
#include "CCamera.h"
#include "ISceneNode.h"
#include "IRenderer.h"
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
	const std::list<ISceneNode*>& sceneNodeList = scene->getSceneNodeList();
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

	//cull renderers
	m_VisbleRenderers.clear();
	const CCamera* cam = scene->get3DCamera();
	for (const auto& node : m_ProcessList)
	{
		const std::list<IRenderer*> rendererList = node->getRendererList();
		for (const IRenderer* renderer : rendererList)
		{
			const aabbox3df& box = renderer->getBoundingBox();
		}
	}

	//scene node renderer
	for (IRenderer* renderer : m_VisbleRenderers)
	{
		ISceneNode* node = renderer->getSceneNode();
		node->render(renderer, cam);
	}

	//actual render
	if (Driver->checkValid())
	{
		Driver->setRenderTarget(nullptr);

		if (Driver->beginScene())
		{
			Driver->clear(true, true, false, BackgroundColor);

			render3D(scene);

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

void CSceneRenderer::render3D(const CScene* scene) const
{
	if (!scene->get3DCamera()->IsInited())
		return;
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
