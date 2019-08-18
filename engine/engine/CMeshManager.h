#pragma once

#include "base.h"
#include <map>

class CMesh;

class CMeshManager
{
private:
	DISALLOW_COPY_AND_ASSIGN(CMeshManager);

public:
	CMeshManager();
	~CMeshManager();

public:


private:
	std::map<std::string, CMesh*>	MeshMap;
};