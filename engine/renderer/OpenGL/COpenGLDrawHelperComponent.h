#pragma once

#include "base.h"

class COpenGLDriver;
class ITexture;
class IVertexBuffer;
class IIndexBuffer;

class COpenGLDrawHelperComponent
{
public:
	explicit COpenGLDrawHelperComponent(COpenGLDriver* driver);
	~COpenGLDrawHelperComponent();

	enum EConstant : uint32_t
	{
		MAX_QUADS = 128,
		MAX_VERTEXCOUNT = 1024 * 3,			//dynamic vertex count
	};

public:
	bool init();

	IVertexBuffer* getStaticVertexBufferScreenQuad() const { return StaticVertexBufferScreenQuad.get(); }
	IVertexBuffer* getStaticVertexBufferScreenQuadFlip() const { return StaticVertexBufferScreenQuadFlip.get(); }

	IIndexBuffer* getStaticIndexBufferQuadList() const { return StaticIndexBufferQuadList.get(); }
	IIndexBuffer* getStaticIndexBufferTriangleList() const { return StaticIndexBufferTriangleList.get(); }

private:
	void createStaticVertexBufferScreenQuad();
	void destroyStaticVertexBufferScreenQuad();

	void createStaticIndexBufferQuadList();
	void destroyStaticIndexBufferQuadList();

	void createStaticIndexBufferTriangleList();
	void destroyStaticIndexBufferTriangleList();

private:
	COpenGLDriver* Driver;

	std::unique_ptr<IVertexBuffer>			StaticVertexBufferScreenQuad;
	std::unique_ptr<IVertexBuffer>			StaticVertexBufferScreenQuadFlip;
	std::unique_ptr<IIndexBuffer>			StaticIndexBufferQuadList;
	std::unique_ptr<IIndexBuffer>			StaticIndexBufferTriangleList;
};