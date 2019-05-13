#pragma once

#include "base.h"
#include <vector>
#include <set>
#include <string>

class CMemFile;

struct SShaderMacro
{
	void addMacro(const std::string& macro)
	{
		macroSet.insert(macro);
	}

	void removeMacro(const std::string& macro)
	{
		macroSet.erase(macro);
	}

	std::set<std::string> macroSet;
};

//从文件中读取shader内容，包括include 注释, 宏处理等
class IShaderFile
{
public:
	IShaderFile() {}
	virtual ~IShaderFile() {}

public:
	virtual bool loadFile(CMemFile* file, const SShaderMacro& shaderMacro) = 0;

protected:
	std::vector<char>  Buffer;
	std::vector<std::string>  IncludeList;
};