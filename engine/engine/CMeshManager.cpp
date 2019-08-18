#include "CMeshManager.h"
#include "CMesh.h"

CMeshManager::CMeshManager()
{
}

CMeshManager::~CMeshManager()
{
	for (auto itr = MeshMap.begin(); itr != MeshMap.end(); ++itr)
	{
		delete itr->second;
	}

	MeshMap.clear();
}
