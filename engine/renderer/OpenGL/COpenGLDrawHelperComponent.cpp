#include "COpenGLDrawHelperComponent.h"

#include "COpenGLDriver.h"
#include "COpenGLVertexIndexBuffer.h"

COpenGLDrawHelperComponent::COpenGLDrawHelperComponent(COpenGLDriver* driver)
	: Driver(driver)
{

}

COpenGLDrawHelperComponent::~COpenGLDrawHelperComponent()
{
	destroyStaticIndexBufferTriangleList();

	destroyStaticIndexBufferQuadList();

	destroyStaticVertexBufferScreenQuad();
}

bool COpenGLDrawHelperComponent::init()
{
	createStaticVertexBufferScreenQuad();

	createStaticIndexBufferQuadList();

	createStaticIndexBufferTriangleList();

	return true;
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
