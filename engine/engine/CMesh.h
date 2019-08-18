#pragma once

#include "aabbox3d.h"
#include "IVertexIndexBuffer.h"
#include <string>

class CMesh
{
public:
	CMesh(IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, E_PRIMITIVE_TYPE primType, uint32_t primCount, const aabbox3df& box)
		: CMesh("", vbuffer, ibuffer, primType, primCount, box) {}
	CMesh(const char* szName, IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, E_PRIMITIVE_TYPE primType, uint32_t primCount, const aabbox3df& box);
	~CMesh();

public:
	void setBoundingBox(const aabbox3df& box) { Box = box; }
	const aabbox3df& getBoundingBox() const { return Box; }

	IVertexBuffer* getVertexBuffer() const { return VBuffer; }
	IIndexBuffer* getIndexBuffer() const { return IBuffer; }

public:
	E_PRIMITIVE_TYPE	PrimType;
	uint32_t	PrimCount;
	std::string		Name;

private:
	IVertexBuffer* VBuffer;
	IIndexBuffer* IBuffer;
	aabbox3df Box;
};