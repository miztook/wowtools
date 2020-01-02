#include "COpenGLVertexIndexBuffer.h"

#include "COpenGLDriver.h"
#include "COpenGLHelper.h"

COpenGLVertexBuffer::COpenGLVertexBuffer(COpenGLDriver* driver, E_MESHBUFFER_MAPPING mapping)
	: IVertexBuffer(mapping), HardwareBuffer(0), Driver(driver)
{
	ASSERT(Driver);
}

COpenGLVertexBuffer::~COpenGLVertexBuffer()
{
	releaseVideoResources();
}

void COpenGLVertexBuffer::do_updateBuffer(const void* data, uint32_t nVerts)
{
	buildVideoResources();

	if (getNumVertices() >= 65536 || nVerts > getNumVertices() || Mapping == EMM_STATIC || !nVerts)
	{
		ASSERT(false);
		return;
	}

	GLenum usage = Mapping == EMM_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	uint32_t vertexSize = getVertexPitchFromType(getVertexType());
	uint32_t sizeToLock = nVerts * vertexSize;
	memcpy(getVertices(), data, sizeToLock);

	{
		Driver->GLExtension.extGlBindBuffer(GL_ARRAY_BUFFER, HardwareBuffer);

		//glBufferSubData真机上效率低下
		Driver->GLExtension.extGlBufferData(GL_ARRAY_BUFFER, sizeToLock, (const GLvoid*)getVertices(), usage);

		Driver->GLExtension.extGlBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

bool COpenGLVertexBuffer::buildVideoResources()
{
	if (HardwareBuffer)
		return true;

	if (!getVertices() || getNumVertices() == 0)
	{
		ASSERT(false);
		return false;
	}

	GLenum usage = GL_DYNAMIC_DRAW;
	switch (Mapping)
	{
	case EMM_STATIC:
	{
		usage = GL_STATIC_DRAW;
	}
	break;
	case EMM_DYNAMIC:
	{
		usage = GL_DYNAMIC_DRAW;
	}
	break;
	case EMM_SOFTWARE:
	{
		usage = GL_DYNAMIC_READ;
	}
	break;
	default:
		ASSERT(false);
		break;
	}

	uint32_t stride = getVertexPitchFromType(getVertexType());
	uint32_t byteWidth = stride * getNumVertices();

	Driver->GLExtension.extGlGenBuffers(1, &HardwareBuffer);

	//create
	Driver->GLExtension.extGlBindBuffer(GL_ARRAY_BUFFER, HardwareBuffer);
	Driver->GLExtension.extGlBufferData(GL_ARRAY_BUFFER, byteWidth, getVertices(), usage);
	Driver->GLExtension.extGlBindBuffer(GL_ARRAY_BUFFER, 0);

	return true;
}


void COpenGLVertexBuffer::releaseVideoResources()
{
	if (HardwareBuffer)
	{
		Driver->deleteVao(this);

		GLuint buffers[] = { HardwareBuffer };
		Driver->GLExtension.extGlDeleteBuffers(1, buffers);

		HardwareBuffer = 0;
	}
}

COpenGLIndexBuffer::COpenGLIndexBuffer(COpenGLDriver* driver, E_MESHBUFFER_MAPPING mapping)
	: IIndexBuffer(mapping), HardwareBuffer(0), Driver(driver)
{
	ASSERT(Driver);
}

COpenGLIndexBuffer::~COpenGLIndexBuffer()
{
	releaseVideoResources();
}

void COpenGLIndexBuffer::do_updateBuffer(const void* data, uint32_t nIndices)
{
	if (getNumIndices() >= 65536 || nIndices > getNumIndices() || Mapping == EMM_STATIC || !nIndices)
	{
		ASSERT(false);
		return;
	}

	GLenum usage = Mapping == EMM_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	uint32_t indexSize = getIndexType() == EIT_16BIT ? 2 : 4;

	uint32_t sizeToLock = nIndices * indexSize;
	memcpy(getIndices(), data, sizeToLock);

	{
		Driver->GLExtension.extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, HardwareBuffer);

		//glBufferSubData真机上效率低下
		Driver->GLExtension.extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeToLock, (const GLvoid*)getIndices(), usage);

		Driver->GLExtension.extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

bool COpenGLIndexBuffer::buildVideoResources()
{
	if (HardwareBuffer)
		return true;

	if (!getIndices() || getNumIndices() == 0)
	{
		ASSERT(false);
		return false;
	}

	GLenum usage = GL_DYNAMIC_DRAW;
	switch (Mapping)
	{
	case EMM_STATIC:
	{
		usage = GL_STATIC_DRAW;
	}
	break;
	case EMM_DYNAMIC:
	{
		usage = GL_DYNAMIC_DRAW;
	}
	break;
	case EMM_SOFTWARE:
	{
		usage = GL_DYNAMIC_READ;
	}
	break;
	default:
		ASSERT(false);
		break;
	}

	uint32_t byteWidth = (getIndexType() == EIT_16BIT ? 2 : 4) * getNumIndices();

	Driver->GLExtension.extGlGenBuffers(1, &HardwareBuffer);

	//create
	Driver->GLExtension.extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, HardwareBuffer);
	Driver->GLExtension.extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, byteWidth, getIndices(), usage);
	Driver->GLExtension.extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

void COpenGLIndexBuffer::releaseVideoResources()
{
	if (HardwareBuffer)
	{
		GLuint buffers[] = { HardwareBuffer };
		Driver->GLExtension.extGlDeleteBuffers(1, buffers);

		HardwareBuffer = 0;
	}
}
