#pragma once

#include "CMaterial.h"
#include <string>

class CMaterialManager
{
private:
	DISALLOW_COPY_AND_ASSIGN(CMaterialManager);

public:
	CMaterialManager();
	~CMaterialManager();

public:
	CMaterial* loadFromFile(const char* filename);

private:
	std::string MaterialDir;
};