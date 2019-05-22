#pragma once

#include "base.h"
#include <vector>
#include <set>
#include <string>

struct SShaderFile 
{
	std::vector<char>  Buffer;
};

//从文件中读取shader内容，包括include 注释, 宏处理等
class CShaderUtil
{
public:
	static bool loadFile_OpenGL(const char* absFileName, const std::set<std::string>& shaderMacro, SShaderFile& result);

	static bool writeFile(const char* buffer, const char* absFileName);

	static std::string getShaderMacroString(const std::set<std::string>& shaderMacro);
	static std::set<std::string> getShaderMacroSet(const char* macroString);

public:
	static const char* getUIPSFileName(bool alpha, bool alphaChannel)
	{
		if (alphaChannel)
		{
			if (alpha)
				return "UI_Alpha_AlphaChannel";
			else
				return "UI_AlphaChannel";
		}
		else
		{
			if (alpha)
				return "UI_Alpha";
			else
				return "UI";
		}
	}

	static const char* getDefaultVSFileName(E_VERTEX_TYPE vType)
	{
		switch (vType)
		{
		case EVT_P:
			return "Default_P";
		case EVT_PC:
			return "Default_PC";
		case EVT_PCT:
			return "Default_PCT";
		case EVT_PN:
			return "Default_PN";
		case EVT_PNC:
			return "Default_PNC";
		case EVT_PNCT:
		case EVT_PNCT2:
			return "Default_PNCT";
		case EVT_PT:
			return "Default_PT";
		case EVT_PNT:
		case EVT_PNT2WA:
			return "Default_PNT";
		default:
			break;
		}
		return "";
	}

	static const char* getDefaultPSFileName(E_VERTEX_TYPE vType)
	{
		switch (vType)
		{
		case EVT_P:
			return "Default_P";
		case EVT_PC:
			return "Default_PC";
		case EVT_PCT:
			return "Default_PCT";
		case EVT_PN:
			return "Default_PN";
		case EVT_PNC:
			return "Default_PNC";
		case EVT_PNCT:
		case EVT_PNCT2:
			return "Default_PNCT";
		case EVT_PT:
			return "Default_PT";
		case EVT_PNT:
		case EVT_PNT2WA:
			return "Default_PNT";
		default:
			break;
		}
		return "";
	}

private:
	static bool processDirectiveInclude(const char* pAfterInclude, const char* szDirInclude, std::string& strInclude);
};
