#include "CMesh.h"

CMesh::CMesh(const char * szName, IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, E_PRIMITIVE_TYPE primType, uint32_t primCount, const aabbox3df & box)
	: Box(box)
{
	Name = szName;
	VBuffer = vbuffer;
	IBuffer = ibuffer;
	primType = primType;
	primCount = primCount;
}

CMesh::~CMesh()
{
	delete VBuffer;
	VBuffer = nullptr;
	delete IBuffer;
	IBuffer = nullptr;
}
