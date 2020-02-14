#include "CCanvas.h"
#include "Engine.h"
#include "IVideoDriver.h"
#include "EngineUtil.h"
#include "CCamera.h"
#include "IVertexIndexBuffer.h"
#include "CShaderUtil.h"

CCanvas::CCanvas()
{
	CPass& pass = Material.addPass(ELM_ALWAYS);

	pass.Cull = ECM_BACK;
	pass.ZWriteEnable = false;
	pass.ZTest = ECFN_NEVER;
	pass.AntiAliasing = EAAM_LINE_SMOOTH;
	pass.VSFile = "";
	pass.PSFile = "UI";
}

CCanvas::~CCanvas()
{

}

void CCanvas::add2DColor(const recti&rect, SColor color, E_2DBlendMode mode /*= E_Solid*/)
{
	ITexture* texture = g_Engine->getDriver()->getTextureWhite();

	S2DBlendParam blendParam(color.getAlpha() < 255, false, mode);
	SQuadDrawBatchKey key(texture, blendParam);
	SQuadBatchDraw& batchDraw = m_2DQuadDrawMap[key];

	{
		SVertex_PCT verts[4];
		verts[0].set(vector3df((float)rect.left, (float)rect.top, 0.0f), color, vector2df(0.0f, 0.0f));
		verts[1].set(vector3df((float)rect.right, (float)rect.top, 0.0f), color, vector2df(1.0f, 0.0f));
		verts[2].set(vector3df((float)rect.left, (float)rect.bottom, 0.0f), color, vector2df(0.0f, 1.0f));
		verts[3].set(vector3df((float)rect.right, (float)rect.bottom, 0.0f), color, vector2df(1.0f, 1.0f));

		batchDraw.drawVerts.emplace_back(verts[0]);
		batchDraw.drawVerts.emplace_back(verts[1]);
		batchDraw.drawVerts.emplace_back(verts[2]);
		batchDraw.drawVerts.emplace_back(verts[3]);
	}
}

void CCanvas::add2DQuads(ITexture* texture, const SVertex_PCT* vertices, uint32_t numQuads, const S2DBlendParam& blendParam /*= S2DBlendParam::OpaqueSource()*/)
{
	ASSERT(texture);
	if (!texture)
		return;

	SQuadDrawBatchKey key(texture, blendParam);
	SQuadBatchDraw& batchDraw = m_2DQuadDrawMap[key];

	const SVertex_PCT* p = vertices;
	for (uint32_t i = 0; i < numQuads; ++i)
	{
		batchDraw.drawVerts.push_back(p[0]);
		batchDraw.drawVerts.push_back(p[1]);
		batchDraw.drawVerts.push_back(p[2]);
		batchDraw.drawVerts.push_back(p[3]);

		p += 4;
	}
}

void CCanvas::renderSubBatch(const CCamera* cam)
{
	for (const auto& kv : m_2DQuadDrawMap)
	{
		const SQuadDrawBatchKey& key = kv.first;
		const SQuadBatchDraw& batchDraw = kv.second;

		uint32_t numQuads = (uint32_t)batchDraw.vertNum() / 4;
		if (!numQuads)
			continue;

		draw2DSquadBatch(cam, key.texture, &batchDraw.drawVerts[0], numQuads, key.blendParam);
	}
	m_2DQuadDrawMap.clear();
}

void CCanvas::draw2DSquadBatch(const CCamera* cam, ITexture* texture, const SVertex_PCT* verts, uint32_t numQuads, const S2DBlendParam& blendParam)
{
	uint32_t nMaxBatch = numQuads / MAX_IMAGE_BATCH_COUNT;
	uint32_t nLeftBatch = numQuads % MAX_IMAGE_BATCH_COUNT;

	for (uint32_t n = 0; n < nMaxBatch; ++n)
	{
		const uint32_t batchCount = MAX_IMAGE_BATCH_COUNT;
		const int nOffset = n * MAX_IMAGE_BATCH_COUNT * 4;

		draw2DSquad(cam, batchCount, texture, &verts[nOffset], batchCount, blendParam);
	}

	if (nLeftBatch)
	{
		const uint32_t batchCount = nLeftBatch;
		const int nOffset = nMaxBatch * MAX_QUADS;

		draw2DSquad(cam, batchCount, texture, &verts[nOffset], batchCount, blendParam);
	}
}

rectf CCanvas::setUVCoords(E_RECT_UVCOORDS uvcoords, float x0, float y0, float x1, float y1)
{
	rectf tcoords;
	switch (uvcoords)
	{
	case ERU_01_10:
	{
		tcoords.left = x0;
		tcoords.top = y1;
		tcoords.right = x1;
		tcoords.bottom = y0;
	}
	break;
	case ERU_10_01:
	{
		tcoords.left = x1;
		tcoords.top = y0;
		tcoords.right = x0;
		tcoords.bottom = y1;
	}
	break;
	case ERU_11_00:
	{
		tcoords.left = x1;
		tcoords.top = y1;
		tcoords.right = x0;
		tcoords.bottom = y0;
	}
	break;
	case  ERU_00_11:
	default:
	{
		tcoords.left = x0;
		tcoords.top = y0;
		tcoords.right = x1;
		tcoords.bottom = y1;
	}
	break;
	}
	return tcoords;
}

void CCanvas::draw2DSquad(const CCamera* cam, uint32_t batchCount, ITexture* texture, const SVertex_PCT* vertices, uint32_t numQuads, const S2DBlendParam& blendParam)
{
	CPass* pass = Material.getPass(ELM_ALWAYS);
	ASSERT(pass);

	IVideoDriver* driver = g_Engine->getDriver();
	IVertexBuffer* vbuffer = driver->getDynamicVertexBuffer();
	IIndexBuffer* ibuffer = driver->getStaticIndexBufferQuadList();

	vbuffer->updateBuffer<SVertex_PCT>(vertices, batchCount * 4);

	applyBlendParam(blendParam, pass);
	Material.setMainTexture(texture);

	SDrawParam drawParam;
	drawParam.numVertices = batchCount * 4;
	drawParam.baseVertIndex = 0;

	driver->setShaderVariable("g_ObjectToWorld", matrix4::Identity());
	driver->setShaderVariable("g_MatrixVP", cam->getVPTM());

	driver->draw(pass, vbuffer, ibuffer,
		EPT_TRIANGLES, batchCount * 2, drawParam);
}

void CCanvas::applyBlendParam(const S2DBlendParam& blendParam, CPass* pass)
{
	pass->clearMacroSet();
	if (blendParam.alpha)
		pass->addMacro("_USE_ALPHA_");
	if (blendParam.alphaChannel)
		pass->addMacro("_USE_ALPHA_CHANNEL_");

	if (!blendParam.alpha && !blendParam.alphaChannel)
	{
		pass->AlphaBlendEnabled = false;
		pass->SrcBlend = EBF_ONE;
		pass->DestBlend = EBF_ZERO;
	}
	else if (blendParam.blendMode == E_AlphaBlend)
	{
		pass->AlphaBlendEnabled = true;
		pass->SrcBlend = EBF_SRC_ALPHA;
		pass->DestBlend = EBF_ONE_MINUS_SRC_ALPHA;
	}
	else if (blendParam.blendMode == E_OneAlpha)
	{
		pass->AlphaBlendEnabled = true;
		pass->SrcBlend = EBF_ONE;
		pass->DestBlend = EBF_ONE_MINUS_SRC_ALPHA;
	}
	else
	{
		pass->AlphaBlendEnabled = false;
		pass->SrcBlend = EBF_ONE;
		pass->DestBlend = EBF_ZERO;
	}
}
