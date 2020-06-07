#include "CSceneRenderer.h"
#include "CScene.h"
#include "Engine.h"
#include "EngineUtil.h"
#include "CCamera.h"
#include "ISceneNode.h"
#include "IRenderer.h"
#include "CRenderLoop.h"
#include "CRenderSetting.h"
#include "ITexture.h"
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
{

}

CSceneRenderer::~CSceneRenderer()
{
	for (auto itr = CameraRenderMap.begin(); itr != CameraRenderMap.end(); ++itr)
	{
		delete itr->second;
	}
	CameraRenderMap.clear();
}

void CSceneRenderer::renderFrame(const CScene* scene, bool active)
{
	beginFrame();

	IVideoDriver* driver = g_Engine->getDriver();
	if (driver->checkValid() && driver->beginScene())
	{
		//3D Camera
		{
			const CCamera* cam = scene->get3DCamera();
			if (cam->IsInited())
			{
				driver->setGlobalMaterial(g_Engine->getRenderSetting()->getGlobalMaterial3D());

				SCameraRender* camRender = getCameraRender(cam);
				camRender->BackgroundColor = SColor(64, 64, 64);


				//ÊÕ¼¯scene node
				camRender->SceneNodeList.clear();
				const std::list<ISceneNode*>& sceneNodeList = scene->getTopSceneNodeList();
				for (ISceneNode* node : sceneNodeList)
				{
					node->traverse([camRender](ISceneNode* n)
					{
						if (n->activeSelf() && !n->isToDelete())
							camRender->SceneNodeList.push_back(n);
					});
				}

				//scene node tick
				const uint32_t tickTime = m_Timer.getTimeSinceLastFrame();
				std::sort(camRender->SceneNodeList.begin(), camRender->SceneNodeList.end(), SceneNodeCompare);
				for (const auto& node : camRender->SceneNodeList)
				{
					node->tick(tickTime, cam);
				}

				//cull renderers
				camRender->CullResult.VisibleRenderers.clear();
				for (const auto& node : camRender->SceneNodeList)
				{
					for (const IRenderer* renderer : node->getRendererList())
					{
						const aabbox3df& box = renderer->getBoundingBox();
						if (cam->getWorldFrustum().isInFrustum(box))
							camRender->CullResult.VisibleRenderers.push_back(renderer);
					}
				}

				//scene node renderer
				camRender->RenderLoop.clearRenderUnits();
				for (const IRenderer* renderer : camRender->CullResult.VisibleRenderers)
				{
					ISceneNode* node = renderer->getSceneNode();
					SRenderUnit* renderUnit = node->render(renderer, cam);
					if (renderUnit)
						camRender->RenderLoop.addRenderUnit(renderUnit);
				}

				//actual render
				{
					driver->setRenderTarget(nullptr);
					driver->clear(true, true, false, camRender->BackgroundColor);

					camRender->RenderLoop.doRenderLoopPrepass(cam);
					camRender->RenderLoop.doRenderLoopForward(cam);
				}

				camRender->RenderLoop.clearRenderUnits();
			}
		}

		//2D Camera
		{
			const CCamera* cam = scene->get2DCamera();
			if (cam->IsInited())
			{
				driver->setGlobalMaterial(g_Engine->getRenderSetting()->getGlobalMaterial2D());

				SCameraRender* camRender = getCameraRender(cam);
				camRender->BackgroundColor = SColor(64, 64, 64);

				CCanvas* canvas = camRender->RenderLoop.getCanvas();

				renderDebugTexture(canvas, scene->getDebugTexture());
				renderDebugInfo(driver, canvas);
				canvas->renderSubBatch(cam);
			}
		}

		driver->endScene();
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

void CSceneRenderer::renderDebugInfo(IVideoDriver* driver, CCanvas* canvas) const
{
	char debugMsg[512];
	Q_sprintf(debugMsg, 512, "Dev: %s\nGraphics: %s\nRes: %d X %d\nFPS: %0.1f\nTriangles: %u\nDraw Call: %u\n",
		driver->AdapterInfo.description.c_str(),
		driver->AdapterInfo.name.c_str(),
		driver->getViewPort().getWidth(),
		driver->getViewPort().getHeight(),
		m_FPSCounter.getFPS(),
		driver->PrimitivesDrawn,
		driver->DrawCall);
	EngineUtil::drawDebugInfo(canvas, debugMsg);
}

void CSceneRenderer::renderDebugTexture(CCanvas* canvas, const ITexture* tex) const
{
	if (!tex)
		return;

	float x = 100;
	float y = 10;
	canvas->add2DImage(tex, rectf(x, y, x + tex->getSize().width, y + tex->getSize().height), 0, S2DBlendParam::OpaqueSource());
}

CSceneRenderer::SCameraRender* CSceneRenderer::getCameraRender(const CCamera* cam)
{
	CSceneRenderer::SCameraRender* camRender = nullptr;
	auto itr = CameraRenderMap.find(cam);
	if (itr == CameraRenderMap.end())
	{
		camRender = new CSceneRenderer::SCameraRender(cam);
		CameraRenderMap[cam] = camRender;
	}
	else
	{
		camRender = itr->second;
	}
	return camRender;
}
