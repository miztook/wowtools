#include "CRenderLoop.h"
#include "CMaterial.h"
#include "IRenderer.h"
#include "Engine.h"
#include "IVideoDriver.h"
#include "CCamera.h"
#include "EngineUtil.h"

bool OpaqueCompare(const SRenderUnit* a, const SRenderUnit* b)
{
	const SGlobalLayerData& layerData1 = a->renderer->getGlobalLayerData();
	const SGlobalLayerData& layerData2 = b->renderer->getGlobalLayerData();

	if (layerData1 != layerData2)
		return layerData1 < layerData2;

	int queue1 = a->renderer->getRenderQueue();
	int queue2 = b->renderer->getRenderQueue();
	if (queue1 != queue2)
		return queue1 < queue2;

	ASSERT(queue1 >= ERQ_INDEX_MIN && queue1 <= ERQ_GEOMETRY_INDEX_MAX);

	//render all first passes first

	//lightmap index

	//shader
	if (a->shaderSortId != b->shaderSortId)
		return a->shaderSortId < b->shaderSortId;

	//material

	//pass

	if (a->distance != b->distance)
		return a->distance > b->distance;

	return a < b;
}

bool AfterOpaqueCompare(const SRenderUnit* a, const SRenderUnit* b)
{
	const SGlobalLayerData& layerData1 = a->renderer->getGlobalLayerData();
	const SGlobalLayerData& layerData2 = b->renderer->getGlobalLayerData();

	if (layerData1 != layerData2)
		return layerData1 < layerData2;

	int queue1 = a->renderer->getRenderQueue();
	int queue2 = b->renderer->getRenderQueue();
	if (queue1 != queue2)
		return queue1 < queue2;

	ASSERT(queue1 > ERQ_GEOMETRY_INDEX_MAX);

	if (a->distance != b->distance)
		return a->distance < b->distance;

	//render all first passes first

	//lightmap index

	//shader
	if (a->shaderSortId != b->shaderSortId)
		return a->shaderSortId < b->shaderSortId;

	//material

	//pass


	return a < b;
}

CRenderLoop::CRenderLoop()
{
	m_Canvas = std::make_unique<CCanvas>();
}

CRenderLoop::~CRenderLoop()
{
	m_Canvas.reset();
}

void CRenderLoop::addRenderUnit(SRenderUnit* unit)
{
	int renderQueue = unit->renderer->getRenderQueue();

	if (renderQueue <= ERQ_GEOMETRY_INDEX_MAX)		//solid
	{
		processRenderUnit(unit);
		m_RenderUnits_Opaque.push_back(unit);
	}
	else if (renderQueue <= ERQ_INDEX_MAX)		//transparent
	{
		processRenderUnit(unit);
		m_RenderUnits_AfterOpaque.push_back(unit);
	}
}

void CRenderLoop::doRenderLoopPrepass(const CCamera* cam)
{

}

void CRenderLoop::doRenderLoopForward(const CCamera* cam)
{
	//opaque
	renderShadowMap(cam);
	collectShadows();
	renderOpaques(cam);
	renderImageEffects(cam);		//处理render target

	//transparent
	renderAfterOpaues(cam);
	renderImageEffects(cam);		//处理render target
}

void CRenderLoop::clearRenderUnits()
{
	for (const SRenderUnit* unit : m_RenderUnits_Opaque)
	{
		delete unit;
	}
	m_RenderUnits_Opaque.clear();

	for (const SRenderUnit* unit : m_RenderUnits_AfterOpaque)
	{
		delete unit;
	}
	m_RenderUnits_AfterOpaque.clear();
}

void CRenderLoop::renderShadowMap(const CCamera* cam)
{

}

void CRenderLoop::collectShadows()
{

}

void CRenderLoop::renderOpaques(const CCamera* cam)
{
	std::sort(m_RenderUnits_Opaque.begin(), m_RenderUnits_Opaque.end(), OpaqueCompare);

	IVideoDriver* driver = g_Engine->getDriver();
	const auto& matView = cam->getViewTM();
	const auto& matProjection = cam->getProjectionTM();
	const auto& matVP = cam->getVPTM();

	for (const SRenderUnit* unit : m_RenderUnits_Opaque)
	{
		if (!unit->primCount)
			continue;

		const IRenderer* renderer = unit->renderer;
		const CPass* pass;
		if (EngineUtil::hasLight(renderer))				//暂时只支持无光照
			pass = nullptr;
		else
			pass = renderer->getMaterial().getPass(ELM_ALWAYS);

		driver->setShaderVariable("g_ObjectToWorld", renderer->getLocalToWorldMatrix());
		driver->setShaderVariable("g_MatrixVP", matVP);
		driver->setShaderVariable("g_MatrixV", matView);
		driver->setShaderVariable("g_MatrixP", matProjection);

		driver->draw(pass, unit->vbuffer, unit->ibuffer, unit->primType, unit->primCount, unit->drawParam);
	}
}

void CRenderLoop::renderImageEffects(const CCamera* cam)
{

}

void CRenderLoop::renderAfterOpaues(const CCamera* cam)
{
	std::sort(m_RenderUnits_AfterOpaque.begin(), m_RenderUnits_AfterOpaque.end(), AfterOpaqueCompare);
}

void CRenderLoop::processRenderUnit(SRenderUnit* unit)
{
	if (unit->vbuffer)
		IVideoResource::buildVideoResources(unit->vbuffer);
	if (unit->ibuffer)
		IVideoResource::buildVideoResources(unit->ibuffer);
}

