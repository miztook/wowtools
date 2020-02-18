#include "CMaterialManager.h"
#include "ScriptCompiler.h"
#include "CFileSystem.h"
#include "stringext.h"

CMaterialManager::CMaterialManager()
{
	MaterialDir = g_FileSystem->getDataDirectory();
	MaterialDir.append("Shaders/materials/");
}

CMaterialManager::~CMaterialManager()
{

}

CMaterial* CMaterialManager::loadFromFile(const char* filename)
{
	std::string absFileName = MaterialDir;
	normalizeDirName(absFileName);
	absFileName.append(filename);

	CReadFile* rf = g_FileSystem->createAndOpenFile(absFileName.c_str(), false);
	if (!rf)
		return nullptr;

	uint32_t len = rf->getSize();
	char* content = new char[len];
	memset(content, 0, len);
	rf->read(content, len);

	CMaterial* mat = nullptr;
	MaterialCompiler compiler;
	if (compiler.parseScript(content, filename))
	{
		const std::map<std::string, CMaterial*>& matMap = compiler.getMaterialMap();
		if (!matMap.empty())
			mat = matMap.begin()->second;
	}

	delete[] content;
	delete rf;

	return mat;
}
