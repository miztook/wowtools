#pragma once

#include "CMaterial.h"

class CMaterialManager
{
private:
	DISALLOW_COPY_AND_ASSIGN(CMaterialManager);

public:
	CMaterialManager();
	~CMaterialManager();

public:
	bool loadFromFile(CMaterial& material, const char* filename);

private:
	std::map<std::string, CMaterial> MaterialMap;
};