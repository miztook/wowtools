#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include "vector3d.h"
#include "S3DVertex.h"

class CMemFile;

class wowWMOFile
{
public:
	struct SWMOBatch 
	{
		uint32_t indexStart;
		uint16_t indexCount;
		uint16_t vertexStart;
		uint16_t vertexEnd;
		uint16_t matId;
		vector3df minPoint;
		vector3df maxPoint;
	};

	struct SWMOBspNode
	{
		uint16_t planetype;		//4: leaf, 0 for YZ-plane, 1 for XZ-plane, 2 for XY-plane?
		int16_t left;
		int16_t right;
		uint16_t numfaces;
		uint16_t firstface;
		float distance;

		uint16_t startIndex;
		uint16_t minVertex;
		uint16_t maxVertex;
	};

public:
	wowWMOFile();
	~wowWMOFile();

public:
	bool loadFile(CMemFile* pMemFile);

	uint16_t getVertexCount(const SWMOBatch& batch) const { return batch.vertexEnd - batch.vertexStart + 1; }

private:
	std::string Name;
};