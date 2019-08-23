#include "CMeshManager.h"
#include "CMesh.h"

CMeshManager::CMeshManager()
{
}

CMeshManager::~CMeshManager()
{
	for (auto & itr : MeshMap)
	{
		delete itr.second;
	}

	MeshMap.clear();
}

void CMeshManager::getCubeMeshVICount(uint32_t& vcount, uint32_t& icount)
{
	vcount = 12;
	icount = 36;
}

bool CMeshManager::fillCubeMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount, const vector3df& size, SColor color /*= SColor::White()*/)
{
	uint32_t vLimit, iLimit;
	getCubeMeshVICount(vLimit, iLimit);

	if (vcount < vLimit || icount < iLimit)
		return false;

	const uint16_t u[36] = { 0, 2, 1, 0, 3, 2, 1, 5, 4, 1, 2, 5, 4, 6, 7, 4, 5, 6,
		7, 3, 0, 7, 6, 3, 9, 5, 2, 9, 8, 5, 0, 11, 10, 0, 10, 7 };

	Q_memcpy(indices, sizeof(uint16_t) * 36, u, sizeof(u));

	vertices[0].set(vector3df(0, 0, 0), color, vector2df(0, 1));
	vertices[1].set(vector3df(1, 0, 0), color, vector2df(1, 1));
	vertices[2].set(vector3df(1, 1, 0), color, vector2df(1, 0));
	vertices[3].set(vector3df(0, 1, 0), color, vector2df(0, 0));
	vertices[4].set(vector3df(1, 0, 1), color, vector2df(0, 1));
	vertices[5].set(vector3df(1, 1, 1), color, vector2df(0, 0));
	vertices[6].set(vector3df(0, 1, 1), color, vector2df(1, 0));
	vertices[7].set(vector3df(0, 0, 1), color, vector2df(1, 1));
	vertices[8].set(vector3df(0, 1, 1), color, vector2df(0, 1));
	vertices[9].set(vector3df(0, 1, 0), color, vector2df(1, 1));
	vertices[10].set(vector3df(1, 0, 1), color, vector2df(1, 0));
	vertices[11].set(vector3df(1, 0, 0), color, vector2df(0, 0));

	for (uint32_t i = 0; i < 12; ++i)
	{
		vertices[i].Pos -= vector3df(0.5f, 0.5f, 0.5f);
		vertices[i].Pos = vertices[i].Pos * size;
	}

	return true;
}

void CMeshManager::getSphereMeshVICount(uint32_t& vcount, uint32_t& icount, uint32_t polyCountX, uint32_t polyCountY)
{
	if (polyCountX < 2)
		polyCountX = 2;
	if (polyCountY < 2)
		polyCountY = 2;
	while (polyCountX * polyCountY > 32767) // prevent uint16_t overflow
	{
		polyCountX /= 2;
		polyCountY /= 2;
	}

	const uint32_t polyCountXPitch = polyCountX + 1; // get to same vertex on next level

	vcount = polyCountXPitch * polyCountY + 2;
	icount = (polyCountX * polyCountY) * 6;
}

bool CMeshManager::fillSphereMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount, float radius /*= 5.f*/, uint32_t polyCountX /*= 16*/, uint32_t polyCountY /*= 16*/, SColor color /*= SColor::White()*/)
{

}
