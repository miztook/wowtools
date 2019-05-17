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

struct SShaderFile 
{
	std::vector<char>  Buffer;
	std::vector<std::string>  IncludeList;
};

//从文件中读取shader内容，包括include 注释, 宏处理等
class CShaderUtil
{
	static const uint32_t MAX_INCLUDE_FILE_SIZE = 4096;
public:
	static bool loadFile(const char* absFileName, const SShaderMacro& shaderMacro, SShaderFile& result);

private:
	static bool processDirectiveInclude(const char* pAfterInclude, const char* szDirInclude, std::string& strInclude);
};
