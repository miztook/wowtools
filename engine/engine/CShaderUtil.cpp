#include "CShaderUtil.h"

#include "CReadFile.h"
#include "CFileSystem.h"
#include "stringext.h"
#include "function.h"

bool CShaderUtil::loadFile_OpenGL(const char* absFileName, const std::set<std::string>& shaderMacro, SShaderFile& result)
{
	CReadFile* rFile = g_FileSystem->createAndOpenFile(absFileName, false);
	if (!rFile)
	{
		ASSERT(false);
		return false;
	}

	char szDirInclude[QMAX_PATH];
	getFileDirA(absFileName, szDirInclude, QMAX_PATH);

	const uint32_t length = rFile->getSize();
	std::vector<char> buffer;
	buffer.resize(length + MAX_INCLUDE_FILE_SIZE);
	rFile->seek(0);

	bool bVersion = false;
	bool bInclude = false;
	bool bEnclose = false;
	bool bMacroAdd = false;
	char* p = buffer.data();
	while (rFile->getPos() < length)
	{
		uint32_t dwRead;
		uint32_t dwLimit = length + MAX_INCLUDE_FILE_SIZE - (uint32_t)(p - buffer.data());
		
		dwRead = rFile->readLine(p, dwLimit);
		if (dwRead == 0)
			break;

		if (dwRead > 1 && !isComment(p))			//skip comment
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
				if (!bInclude)
				{
					std::string token;
					if (getToken(p, token) && token == "#include")
					{
						if (processDirectiveInclude(p + token.length(), szDirInclude, strInclude))
							bInclude = true;
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

			Q_strcpy(p, macroString.length() + 1, macroString.c_str());
			p += macroString.length();

			bMacroAdd = true;
		}
	}

	ASSERT(p <= buffer.data() + length + MAX_INCLUDE_FILE_SIZE);

	result.Buffer = buffer;
	return true;
}

std::string CShaderUtil::getVSDir(const char * profile)
{
	std::string str = g_FileSystem->getDataDirectory();
	str.append("Shaders/Vertex/");
	str.append(profile);

	return str;
}

std::string CShaderUtil::getPSDir(const char * profile)
{
	std::string str = g_FileSystem->getDataDirectory();
	str.append("Shaders/Pixel/");
	str.append(profile);

	return str;
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

		if (rFile->getSize() >= MAX_INCLUDE_FILE_SIZE)
		{
			g_FileSystem->writeLog(ELOG_GX, "CShaderUtil::processDirectiveInclude Failed: File Size too large: %s", absFileName.c_str());
			ASSERT(false);
			return false;
		}

		const uint32_t length = rFile->getSize();
		std::vector<char> buffer;
		buffer.resize(length);
		memset(buffer.data(), 0, length);
		rFile->seek(0);

		bool bEnclose = false;
		char* p = buffer.data();
		while (rFile->getPos() < length)
		{
			uint32_t dwRead = rFile->readLine(p, (uint32_t)(length - (p - buffer.data())));
			if (dwRead == 0)
				break;

			if (dwRead > 1 && !isComment(p))			//skip comment
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
					p[dwRead - 1] = '\n';		//replace '\0' to '\n'
					p += dwRead;
				}
			}
		}

		ASSERT(p <= buffer.data() + length);

		strInclude = std::string(buffer.data(), p - buffer.data());

		return true;
	}
	return false;
}
