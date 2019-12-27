#include "COpenGLDrawHelperComponent.h"

#include "COpenGLDriver.h"
#include "COpenGLVertexIndexBuffer.h"
#include "ITexture.h"
#include "EngineUtil.h"
#include "Engine.h"

COpenGLDrawHelperComponent::COpenGLDrawHelperComponent(COpenGLDriver* driver)
	: Driver(driver)
{

}

COpenGLDrawHelperComponent::~COpenGLDrawHelperComponent()
{
	destroyDynamicVertexBuffers();

	destroyStaticIndexBufferTriangleList();

	destroyStaticIndexBufferQuadList();

	destroyStaticVertexBufferScreenQuad();
}

bool COpenGLDrawHelperComponent::init()
{
	createStaticVertexBufferScreenQuad();

	createStaticIndexBufferQuadList();

	createStaticIndexBufferTriangleList();

	createDynamicVertexBuffers();

	initMaterials();

	return true;
}

void COpenGLDrawHelperComponent::initMaterials()
{
	InitMaterial2D.Cull = ECM_BACK;
	InitMaterial2D.ZWriteEnable = false;
	InitMaterial2D.ZBuffer = ECFN_NEVER;
	InitMaterial2D.AntiAliasing = EAAM_LINE_SMOOTH;
}

void COpenGLDrawHelperComponent::createStaticVertexBufferScreenQuad()
{
	StaticVertexBufferScreenQuad = std::make_unique<COpenGLVertexBuffer>(Driver, EMM_STATIC);
	{
		SVertex_PT* vertices = StaticVertexBufferScreenQuad->alloc<SVertex_PT>(4);
		vertices[0].Pos.set(-1.f, 1.f, 0.0f);
		vertices[0].TCoords.set(0, 0);
		vertices[1].Pos.set(1.f, 1.f, 0.0f);
		vertices[1].TCoords.set(1, 0);
		vertices[2].Pos.set(-1.f, -1.f, 0.0f);
		vertices[2].TCoords.set(0, 1);
		vertices[3].Pos.set(1.f, -1.f, 0.0f);
		vertices[3].TCoords.set(1, 1);
	}

	StaticVertexBufferScreenQuadFlip = std::make_unique<COpenGLVertexBuffer>(Driver, EMM_STATIC);
	{
		SVertex_PT* vertices = StaticVertexBufferScreenQuadFlip->alloc<SVertex_PT>(4);
		vertices[0].Pos.set(-1.f, 1.f, 0.0f);
		vertices[0].TCoords.set(0, 1);
		vertices[1].Pos.set(1.f, 1.f, 0.0f);
		vertices[1].TCoords.set(1, 1);
		vertices[2].Pos.set(-1.f, -1.f, 0.0f);
		vertices[2].TCoords.set(0, 0);
		vertices[3].Pos.set(1.f, -1.f, 0.0f);
		vertices[3].TCoords.set(1, 0);
	}
}

void COpenGLDrawHelperComponent::destroyStaticVertexBufferScreenQuad()
{
	StaticVertexBufferScreenQuadFlip.reset();
	StaticVertexBufferScreenQuad.reset();
}

void COpenGLDrawHelperComponent::createStaticIndexBufferQuadList()
{
	StaticIndexBufferQuadList = std::make_unique<COpenGLIndexBuffer>(Driver, EMM_STATIC);
	{
		uint16_t* indices = StaticIndexBufferQuadList->alloc<uint16_t>(MAX_QUADS * 6);

		uint32_t firstVert = 0;
		uint32_t firstIndex = 0;
		for (uint32_t i = 0; i < MAX_QUADS; ++i)
		{
			indices[firstIndex + 0] = (uint16_t)firstVert + 0;
			indices[firstIndex + 1] = (uint16_t)firstVert + 1;
			indices[firstIndex + 2] = (uint16_t)firstVert + 2;
			indices[firstIndex + 3] = (uint16_t)firstVert + 3;
			indices[firstIndex + 4] = (uint16_t)firstVert + 2;
			indices[firstIndex + 5] = (uint16_t)firstVert + 1;

			firstVert += 4;
			firstIndex += 6;
		}
	}
}

void COpenGLDrawHelperComponent::destroyStaticIndexBufferQuadList()
{
	StaticIndexBufferQuadList.reset();
}

void COpenGLDrawHelperComponent::createStaticIndexBufferTriangleList()
{
	StaticIndexBufferTriangleList = std::make_unique<COpenGLIndexBuffer>(Driver, EMM_STATIC);
	{
		uint16_t* indices = StaticIndexBufferTriangleList->alloc<uint16_t>(MAX_VERTEXCOUNT);
		for (uint32_t i = 0; i < MAX_VERTEXCOUNT; ++i)
		{
			indices[i] = i;
		}
	}
}

void COpenGLDrawHelperComponent::destroyStaticIndexBufferTriangleList()
{
	StaticIndexBufferTriangleList.reset();
}

void COpenGLDrawHelperComponent::createDynamicVertexBuffers()
{
	VBImage = std::make_unique<COpenGLVertexBuffer>(Driver, EMM_DYNAMIC);
	VBImage->alloc<SVertex_PCT>(MAX_QUADS * 4);
}

void COpenGLDrawHelperComponent::destroyDynamicVertexBuffers()
{
	VBImage.reset();
}

rectf COpenGLDrawHelperComponent::setUVCoords(E_RECT_UVCOORDS uvcoords, float x0, float y0, float x1, float y1)
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

void COpenGLDrawHelperComponent::add2DColor(const recti& rect, SColor color, E_2DBlendMode mode)
{
	ITexture* texture = Driver->getTextureWhite();

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

void COpenGLDrawHelperComponent::add2DQuads(ITexture* texture, const SVertex_PCT* vertices, uint32_t numQuads, const S2DBlendParam & blendParam)
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

void COpenGLDrawHelperComponent::flushAll2DQuads()
{
	for (const auto& kv : m_2DQuadDrawMap)
	{
		const SQuadDrawBatchKey& key = kv.first;
		const SQuadBatchDraw& batchDraw = kv.second;

		uint32_t numQuads = (uint32_t)batchDraw.vertNum() / 4;
		if (!numQuads)
			continue;

		draw2DSquadBatch(key.texture, &batchDraw.drawVerts[0], numQuads, key.blendParam);
	}
	m_2DQuadDrawMap.clear();
}

void COpenGLDrawHelperComponent::draw2DSquadBatch(ITexture * texture, const SVertex_PCT* verts, uint32_t numQuads, const S2DBlendParam & blendParam)
{
	uint32_t nMaxBatch = numQuads / MAX_IMAGE_BATCH_COUNT;
	uint32_t nLeftBatch = numQuads % MAX_IMAGE_BATCH_COUNT;

	for (uint32_t n = 0; n < nMaxBatch; ++n)
	{
		const uint32_t batchCount = MAX_IMAGE_BATCH_COUNT;
		const int nOffset = n * MAX_IMAGE_BATCH_COUNT * 4;

		draw2DSquad(batchCount, texture, &verts[nOffset], batchCount, blendParam);
	}

	if (nLeftBatch)
	{
		const uint32_t batchCount = nLeftBatch;
		const int nOffset = nMaxBatch * MAX_QUADS;
		
		draw2DSquad(batchCount, texture, &verts[nOffset], batchCount, blendParam);
	}
}

void COpenGLDrawHelperComponent::draw2DSquad(uint32_t batchCount, ITexture* texture, const SVertex_PCT* vertices, uint32_t numQuads, const S2DBlendParam& blendParam)
{
	VBImage->updateBuffer<SVertex_PCT>(vertices, batchCount * 4);

	InitMaterial2D.setMainTexture(texture);
	blendParam.setMaterial(InitMaterial2D);

	Driver->setMaterial(InitMaterial2D);
	Driver->setGlobalMaterial(g_Engine->getRenderSetting().getGlobalMaterial2D());

	IVertexBuffer* vbuffer = VBImage.get();
	IIndexBuffer* ibuffer = getStaticIndexBufferQuadList();
	EngineUtil::buildVideoResources(vbuffer, ibuffer, &InitMaterial2D);

	SDrawParam drawParam;
	drawParam.numVertices = batchCount * 4;
	drawParam.baseVertIndex = 0;

	Driver->setShaderVariable("g_ObjectToWorld", matrix4::Identity());
	Driver->setShaderVariable("g_MatrixVP", Driver->M_VP2D);

	Driver->draw(vbuffer, ibuffer, EPT_TRIANGLES,
		batchCount * 2, drawParam, true);
}
