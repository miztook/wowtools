#pragma once

#include "base.h"
#include <vector>
#include <set>
#include <string>
#include <map>

struct SShaderFile 
{
	std::vector<char>  Buffer;
};

//从文件中读取shader内容，包括include 注释, 宏处理等
class CShaderUtil
{
public:
	static bool loadFile_OpenGL(const char* absFileName, const char* shaderMacro, SShaderFile& result);

	static bool writeFile(const char* buffer, const char* absFileName);

	static std::string getShaderMacroString(const char* macro0, const char* macro1, const char* macro2, const char* macro3);
	static std::string getShaderMacroString(const std::set<std::string>& macroSet);
	static void getShaderMacroSet(const char* macroString, std::set<std::string>& macroSet);

public:
	struct SShaderKey
	{
		SShaderKey(const char* vsFile, const char* psFile, const char* macroString)
			: VSFile(vsFile), PSFile(psFile), MacroString(macroString) {}

		std::string		VSFile;
		std::string		PSFile;
		std::string		MacroString;

		bool operator<(const SShaderKey& other) const
		{
			if (VSFile != other.VSFile)
				return VSFile < other.VSFile;
			if (PSFile != other.PSFile)
				return PSFile < other.PSFile;
			else
				return MacroString < other.MacroString;
		}
	};

	static SShaderKey getShaderKey(const char* vsFile, const char* psFile, const char* macroString, E_VERTEX_TYPE vertexType);
	static int getShaderProgramSortId(const char* vsFile, const char* psFile, const char* macroString, E_VERTEX_TYPE vertexType);

	static const char* getDefaultVSFileName(E_VERTEX_TYPE vType);

	static const char* getDefaultPSFileName(E_VERTEX_TYPE vType);

private:
	static bool processDirectiveInclude(const char* pAfterInclude, const char* szDirInclude, std::string& strInclude);

	static std::map<SShaderKey, int> ShaderIdMap;
};
