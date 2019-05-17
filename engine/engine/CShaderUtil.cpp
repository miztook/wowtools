#include "CShaderUtil.h"

#include "CReadFile.h"
#include "CFileSystem.h"
#include "stringext.h"

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
		//∂¡»°include
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
				//¥¶¿Ì/*  */
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
