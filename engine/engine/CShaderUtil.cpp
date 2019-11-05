#include "CShaderUtil.h"

#include "CReadFile.h"
#include "CWriteFile.h"
#include "CFileSystem.h"
#include "stringext.h"
#include "function.h"

std::map<CShaderUtil::SShaderKey, int> CShaderUtil::ShaderIdMap;

bool CShaderUtil::loadFile_OpenGL(const char* absFileName, const std::set<std::string>& shaderMacro, SShaderFile& result)
{
	CReadFile* rFile = g_FileSystem->createAndOpenFile(absFileName, false);
	if (!rFile)
	{
		ASSERT(false);
		return false;
	}

	const uint32_t length = rFile->getSize();
	std::vector<char> buffer;
	buffer.resize(length);
	memset(buffer.data(), 0, buffer.size());
	rFile->seek(0);

	bool bVersion = false;
	bool bEnclose = false;
	bool bMacroAdd = false;
	char* p = buffer.data();
	while (rFile->getPos() < length)
	{
		uint32_t dwRead;
		uint32_t dwLimit = (uint32_t)(buffer.size() - (p - buffer.data()));
		
		memset(p, 0, dwLimit);
		dwRead = rFile->readLine(p, dwLimit);
		if (dwRead == 0)
			break;

		if (!isComment(p))			//skip comment
		{
			//处理/*  */
			bool bStart = isEnclosedStart(p);
			bool bEnd = isEnclosedEnd(p);
			if (bStart)
			{
				if (!bEnclose)
					bEnclose = true;
			}
			if (bEnd)
			{
				if (bEnclose)
					bEnclose = false;
			}

			if (!bEnclose && !bStart && !bEnd)
			{
				if (!bVersion)
				{
					std::string token;
					if (getToken(p, token) && token == "#version")
						bVersion = true;
				}

				std::string strInclude;
				bool bInclude = false;
				{
					std::string token;
					if (getToken(p, token) && token == "#include")
					{
						char szDirInclude[QMAX_PATH];
						getFileDirA(absFileName, szDirInclude, QMAX_PATH);

						if (processDirectiveInclude(p + token.length(), szDirInclude, strInclude))
						{
							bInclude = true;
							
							//expand buffer
							{
								const size_t offset = p - buffer.data();
								std::vector<char> newBuffer;
								newBuffer.resize(buffer.size() + strInclude.length() + 1);
								memcpy(newBuffer.data(), buffer.data(), offset);
								buffer = newBuffer;
								p = buffer.data() + offset;
							}
						}
					}
				}

				if (bInclude && !strInclude.empty())		//添加include
				{
					size_t len = strInclude.length();
					Q_strcpy(p, len + 1, strInclude.c_str());
					p[len] = '\n';				//replace '\0' to '\n'		
					p += (len + 1);
				}
				else
				{
					p[dwRead - 1] = '\n';		//replace '\0' to '\n'
					p += dwRead;
				}
			}
		}

		if (bVersion && !bMacroAdd && !shaderMacro.empty())
		{
			std::string macroString;
			for (const auto& macro : shaderMacro)
			{
				macroString.append("#define ");
				macroString.append(macro);
				macroString.append("\n");
			}

			//expand buffer
			{
				const size_t offset = p - buffer.data();
				std::vector<char> newBuffer;
				newBuffer.resize(buffer.size() + macroString.length() + 1);
				memcpy(newBuffer.data(), buffer.data(), offset);
				buffer = newBuffer;
				p = buffer.data() + offset;
			}

			Q_strcpy(p, macroString.length() + 1, macroString.c_str());
			p += macroString.length();

			bMacroAdd = true;
		}
	}

	delete rFile;

	ASSERT(p <= buffer.data() + buffer.size());

	/*
	std::string outFile = g_FileSystem->getDataDirectory();
	outFile += "tmp.txt";
	writeFile(buffer.data(), outFile.c_str());
	*/

	result.Buffer = buffer;
	return true;
}

bool CShaderUtil::writeFile(const char* buffer, const char* absFileName)
{
	CWriteFile* wfile = g_FileSystem->createAndWriteFile(absFileName, false);
	if (!wfile)
	{
		ASSERT(false);
		return false;
	}

	wfile->writeText(buffer);

	delete wfile;
	return true;
}

bool CShaderUtil::processDirectiveInclude(const char* pAfterInclude, const char* szDirInclude, std::string& strInclude)
{
	std::string absFileName = szDirInclude;
	normalizeDirName(absFileName);

	std::string token;
	if (!getToken(pAfterInclude, token))
		return false;

	const char* pszTokenStart = token.c_str();
	const char* pszTokenEnd = pszTokenStart + token.length() - 1;
	if ((pszTokenEnd > pszTokenStart + 1) &&
		*pszTokenStart == '\"' && *pszTokenEnd == '\"')
	{
		//读取include
		std::string strIncludeFileName(pszTokenStart + 1, pszTokenEnd - pszTokenStart - 1);
		absFileName.append(strIncludeFileName);

		CReadFile* rFile = g_FileSystem->createAndOpenFile(absFileName.c_str(), false);
		if (!rFile)
		{
			g_FileSystem->writeLog(ELOG_GX, "CShaderUtil::processDirectiveInclude Failed: Shader missing: %s", absFileName.c_str());
			ASSERT(false);
			return false;
		}

		const uint32_t length = rFile->getSize();
		std::vector<char> buffer;
		buffer.resize(length);
		rFile->seek(0);

		bool bEnclose = false;
		char* p = buffer.data();
		while (rFile->getPos() < length)
		{
			uint32_t dwLimit = (uint32_t)(buffer.size() - (p - buffer.data()));
			uint32_t dwRead = rFile->readLine(p, dwLimit);
			if (dwRead == 0)
				break;

			if (!isComment(p))			//skip comment
			{
				//处理/*  */
				bool bStart = isEnclosedStart(p);
				bool bEnd = isEnclosedEnd(p);
				if (bStart)
				{
					if (!bEnclose)
						bEnclose = true;
				}
				if (bEnd)
				{
					if (bEnclose)
						bEnclose = false;
				}

				if (!bEnclose && !bStart && !bEnd)
				{
					std::string strInclude;
					bool bInclude = false;
					{
						std::string token;
						if (getToken(p, token) && token == "#include")
						{
							char szDirInclude[QMAX_PATH];
							getFileDirA(absFileName.c_str(), szDirInclude, QMAX_PATH);

							if (processDirectiveInclude(p + token.length(), szDirInclude, strInclude))
							{
								bInclude = true;

								//expand buffer
								{
									const size_t offset = p - buffer.data();
									std::vector<char> newBuffer;
									newBuffer.resize(buffer.size() + strInclude.length() + 1);
									memcpy(newBuffer.data(), buffer.data(), offset);
									buffer = newBuffer;
									p = buffer.data() + offset;
								}
							}
						}
					}

					if (bInclude && !strInclude.empty())		//添加include
					{
						size_t len = strInclude.length();
						Q_strcpy(p, len + 1, strInclude.c_str());
						p[len] = '\n';				//replace '\0' to '\n'		
						p += (len + 1);
					}
					else
					{
						p[dwRead - 1] = '\n';		//replace '\0' to '\n'
						p += dwRead;
					}
				}
			}
		}

		delete rFile;

		ASSERT(p <= buffer.data() + buffer.size());

		strInclude = std::string(buffer.data(), p - buffer.data());

		return true;
	}
	return false;
}

std::string CShaderUtil::getShaderMacroString(const std::set<std::string>& shaderMacro)
{
	std::string str;

	size_t count = 0;
	for (const auto& macro : shaderMacro)
	{
		str += macro;
		if (count + 1 < shaderMacro.size())
		{
			str += " ";
		}
		++count;
	}
	return str;
}

std::set<std::string> CShaderUtil::getShaderMacroSet(const char* macroString)
{
	std::set<std::string> macroSet;
	std::vector<std::string> macroList;
	std_string_split(macroString, '#', macroList);
	for (const auto& str : macroList)
	{
		if (!str.empty())
			macroSet.insert(str);
	}
	return macroSet;
}

CShaderUtil::SShaderKey CShaderUtil::getShaderKey(const char* vsFile, const char* vsMacroString, const char* psFile, const char* psMacroString, E_VERTEX_TYPE vertexType)
{
	SShaderKey key(vsFile, vsMacroString, psFile, psMacroString);
	if (key.VSFile.empty())
		key.VSFile = getDefaultVSFileName(vertexType);
	if (key.PSFile.empty())
		key.PSFile = getDefaultPSFileName(vertexType);
	return key;
}

int CShaderUtil::getShaderProgramSortId(const char* vsFile, const char* vsMacroString, const char* psFile, const char* psMacroString, E_VERTEX_TYPE vertexType)
{
	SShaderKey key = getShaderKey(vsFile, vsMacroString, psFile, psMacroString, vertexType);

	auto itr = ShaderIdMap.find(key);
	if (itr == ShaderIdMap.end())
	{
		int id = (int)ShaderIdMap.size() + 1;
		ShaderIdMap[key] = id;
		return id;
	}
	return itr->second;
}

std::string CShaderUtil::getUIPSMacroString(bool alpha, bool alphaChannel)
{
	if (alphaChannel)
	{
		if (alpha)
			return std::string("_USE_ALPHA_") + "#" + std::string("_USE_ALPHA_CHANNEL_");
		else
			return std::string("_USE_ALPHA_CHANNEL_");
	}
	else
	{
		if (alpha)
			return std::string("_USE_ALPHA_");
		else
			return "";
	}
}

const char* CShaderUtil::getDefaultVSFileName(E_VERTEX_TYPE vType)
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

const char* CShaderUtil::getDefaultPSFileName(E_VERTEX_TYPE vType)
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
