#pragma once

#include "COpenGLPCH.h"

#include "IVertexIndexBuffer.h"

class COpenGLDriver;

class COpenGLVertexBuffer : public IVertexBuffer
{
public:
	COpenGLVertexBuffer(COpenGLDriver* driver, E_MESHBUFFER_MAPPING mapping);
	virtual ~COpenGLVertexBuffer();

public:
	GLuint getHWBuffer() const { return HardwareBuffer; }

protected:
	//IVideoResource
	bool buildVideoResources() override;
	void releaseVideoResources() override;
	bool hasVideoBuilt() const override { return HardwareBuffer != 0; }

	void do_updateBuffer(const void* data, uint32_t nVerts) override;

private:
	COpenGLDriver*		Driver;

	GLuint	HardwareBuffer;
};

class COpenGLIndexBuffer : public IIndexBuffer
{
public:
	COpenGLIndexBuffer(COpenGLDriver* driver, E_MESHBUFFER_MAPPING mapping);
	virtual ~COpenGLIndexBuffer();

public:
	GLuint getHWBuffer() const { return HardwareBuffer; }

protected:
	//IVideoResource
	bool buildVideoResources() override;
	void releaseVideoResources() override;
	bool hasVideoBuilt() const override { return HardwareBuffer != 0; }

	void do_updateBuffer(const void* data, uint32_t nIndices) override;
private:
	COpenGLDriver*		Driver;

	GLuint	HardwareBuffer;
};
