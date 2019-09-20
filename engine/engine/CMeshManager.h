#pragma once

#include "base.h"
#include <map>
#include "aabbox3d.h"
#include "S3DVertex.h"
#include "IVertexIndexBuffer.h"

class CMesh;

class CMeshManager
{
private:
	DISALLOW_COPY_AND_ASSIGN(CMeshManager);

public:
	CMeshManager();
	~CMeshManager();

public:
	bool addMesh(const char* name, IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, E_PRIMITIVE_TYPE primType, uint32_t primCount, const aabbox3df& box);
	const CMesh* getMesh(const char* name) const;
	void removeMesh(const char* name);

	bool addGridLineMesh(const char* name, uint32_t xzCount, float gridSize, SColor color);
	bool addDecal(const char* name, float width, float height, SColor color);
	bool addCube(const char* name, const vector3df& size, SColor color);
	bool addSphere(const char* name, float radius, uint32_t polyCountX, uint32_t polyCountY, SColor color);
	bool addCylinder(const char* name, float radius, float height, uint32_t numSegment, SColor color);

public:
	static void getCubeMeshVICount(uint32_t& vcount, uint32_t& icount);
	static bool fillCubeMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount,
		const vector3df& size, SColor color = SColor::White());
	static aabbox3df getCubeMeshAABBox(const vector3df& size) { return aabbox3df(-size / 2, size / 2); }

	static void getSphereMeshVICount(uint32_t& vcount, uint32_t& icount, uint32_t polyCountX, uint32_t polyCountY);
	static bool fillSphereMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount,
		float radius = 5.f,
		uint32_t polyCountX = 16, uint32_t polyCountY = 16, SColor color = SColor::White());
	static aabbox3df getSphereMeshAABBox(float radius)
	{
		return aabbox3df(vector3df(-radius, -radius, -radius), vector3df(radius, radius, radius));
	}

	static void getCylinderMeshVICount(uint32_t& vcount, uint32_t& icount, uint32_t numSegment);
	static bool fillCylinderMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount,
		float radius, float height, uint32_t numSegment = 20, SColor color = SColor());
	static aabbox3df getCylinderMeshAABBox(float radius, float height)
	{
		return aabbox3df(vector3df(-radius, -height, -radius), vector3df(radius, height, radius));
	}

	static uint32_t getGridLineVCount(uint32_t xzCount);
	static bool fillGridLineMeshV(SVertex_PC* gVertices, uint32_t vcount, uint32_t xzCount, float gridSize, SColor color);
	static aabbox3df getGridLineAABBox(uint32_t xzCount, float gridSize)
	{
		float halfWidth = xzCount * gridSize;
		return aabbox3df(vector3df(-halfWidth, 0, -halfWidth), vector3df(halfWidth, 0, halfWidth));
	}

private:
	std::map<std::string, CMesh*>	MeshMap;
};