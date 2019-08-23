#pragma once

#include "base.h"
#include <map>
#include "aabbox3d.h"
#include "S3DVertex.h"

class CMesh;

class CMeshManager
{
private:
	DISALLOW_COPY_AND_ASSIGN(CMeshManager);

public:
	CMeshManager();
	~CMeshManager();

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

	static uint32_t getGridLineVCount(uint32_t xzCount) { return (2 * xzCount + 1) * 4; }
	static bool fillGridLineMeshV(SVertex_PC* gVertices, uint32_t vcount, uint32_t xzCount, float gridSize, SColor color);
	static aabbox3df getGridLineAABBox(uint32_t xzCount, float gridSize)
	{
		float halfWidth = xzCount * gridSize;
		return aabbox3df(vector3df(-halfWidth, 0, -halfWidth), vector3df(halfWidth, 0, halfWidth));
	}

private:
	std::map<std::string, CMesh*>	MeshMap;
};