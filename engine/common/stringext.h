#pragma once

#include "predefine.h"
#include <algorithm>
#include <string>
#include <vector>
#include <cassert>

inline bool isAbsoluteFileName(const std::string& filename)
{
	unsigned int len = (unsigned int)filename.length();
#ifdef  A_PLATFORM_WIN_DESKTOP
	if (len >= 2)
		return filename[1] == ':';
#else
	if (len >= 1)
		return filename[0] == '/';
#endif

	return false;
}

inline void str_toupper(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

inline void str_tolower(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

inline void normalizeFileName(std::string& filename)
{
	unsigned int len = (unsigned int)filename.length();
	for (unsigned int i = 0; i < len; ++i)
	{
		if (filename[i] == '\\')
			filename[i] = '/';
	}
}

inline void normalizeDirName(std::string& dirName)
{
	unsigned int len = (unsigned int)dirName.length();
	for (unsigned int i = 0; i < len; ++i)
	{
		if (dirName[i] == '\\')
			dirName[i] = '/';
	}

	if (len > 0)
	{
		char last = dirName.back();
		if (last != '/' && last != '\\')
			dirName.append("/");
	}
}

inline bool isNormalized(const std::string& filename)
{
	unsigned int len = (unsigned int)filename.length();
	for (unsigned int i = 0; i < len; ++i)
	{
		if (filename[i] == '\\')
			return false;
	}
	return true;
}

inline bool isLowerFileName(const std::string& filename)
{
	unsigned int len = (unsigned int)filename.length();
	for (unsigned int i = 0; i < len; ++i)
	{
		if (isupper(filename[i]))
			return false;
	}
	return true;
}

inline bool isSpace(char ch)
{
	return ch == '\t' || ch == '\r' || ch == ' ';
}

inline bool isComment(const char* p)
{
	uint32_t len = (uint32_t)strlen(p);
	for (uint32_t i = 0; i < len; ++i)
	{
		if (isSpace(p[i]))
			continue;

		if (i + 1 < len && p[i] == '/' && p[i + 1] == '/')
			return true;
		else
			return false;
	}
	return false;
}

inline bool isEnclosedStart(const char* p)
{
	uint32_t len = (uint32_t)strlen(p);
	for (uint32_t i = 0; i < len; ++i)
	{
		if (isSpace(p[i]))
			continue;

		if (i + 1 < len && p[i] == '/' && p[i + 1] == '*')
			return true;
		else
			return false;
	}
	return false;
}

inline bool isEnclosedEnd(const char* p)
{
	int len = (int)strlen(p);
	for (int i = len - 1; i >= 0; --i)
	{
		if (isSpace(p[i]))
			continue;

		if (i - 1 >= 0 && p[i - 1] == '*' && p[i] == '/')
			return true;
		else
			return false;
	}
	return false;
}

//make multiple #define MACRO, split by #
inline void makeMacroString(std::string& macroString, const char* strMacro)
{
	macroString.clear();
	uint32_t len = (uint32_t)strlen(strMacro);
	if (len == 0)
		return;

	uint32_t p = 0;
	for (uint32_t i = 0; i < len; ++i)
	{
		if (strMacro[i] == '#')	//split
		{
			if (i - p > 0)
			{
				std::string str(&strMacro[p], i - p);
				macroString.append("#define ");
				macroString.append(str.c_str());
				macroString.append("\n");
			}
			p = i + 1;
		}
	}

	//last
	if (len - p > 0)
	{
		std::string str(&strMacro[p], len - p);
		macroString.append("#define ");
		macroString.append(str.c_str());
		macroString.append("\n");
	}
}

inline void makeMacroStringList(std::vector<std::string>& macroStrings, const char* strMacro)
{
	macroStrings.clear();
	uint32_t len = (uint32_t)strlen(strMacro);
	if (len == 0)
		return;

	uint32_t p = 0;
	for (uint32_t i = 0; i < len; ++i)
	{
		if (i > 0 && strMacro[i] == '#')	//split
		{
			if (i - p > 0)
			{
				std::string str(&strMacro[p], i - p);
				p = i + 1;
				macroStrings.push_back(str.c_str());
			}
		}
	}

	//last
	if (len - p > 0)
	{
		std::string str(&strMacro[p], len - p);
		macroStrings.push_back(str.c_str());
	}
}

inline bool getToken(const char* pszLine, std::string& strToken)
{
	const char* pChar = pszLine;

	while (isSpace(*pChar))
	{
		++pChar;
	}

	const char* pszTokenStart = pChar;

	// If it starts with ", then it's a string, and we should end searching until found another ".
	if (*pszTokenStart == '\"')
	{
		++pChar;

		while (*pChar != '\"' && *pChar != 0 && *pChar != '\n')
		{
			++pChar;
		}

		if (*pChar == '\"')
		{
			++pChar;
		}
	}
	// If it's not a string.
	else
	{
		while (!isSpace(*pChar) && *pChar != 0 && *pChar != '\n')
		{
			++pChar;
		}
	}

	const char* pszTokenEnd = pChar;

	if (pszTokenEnd == pszTokenStart)
	{
		return false;
	}

	strToken = std::string(pszTokenStart, pszTokenEnd - pszTokenStart);
	return true;
}

inline void trim(std::string& s, const std::string& drop)
{
	// trim right
	s.erase(s.find_last_not_of(drop) + 1);
	// trim left
	s.erase(0, s.find_first_not_of(drop));
}

inline void ltrim(std::string& s, const std::string& drop)
{
	// trim left
	s.erase(0, s.find_first_not_of(drop));
}
inline void rtrim(std::string& s, const std::string& drop)
{
	// trim right
	s.erase(s.find_last_not_of(drop) + 1);
}

inline std::string std_string_format(const char* _Format, ...) {
	std::string tmp;

	va_list marker = NULL;
	va_start(marker, _Format);

	int num_of_chars = vsnprintf(nullptr, 0, _Format, marker);
	tmp.resize(num_of_chars);

	vsprintf((char *)tmp.data(), _Format, marker);

	va_end(marker);

	return tmp;
}

inline void std_string_split(const std::string& _str, char split, std::vector<std::string>& retVString)
{
	retVString.clear();

	const char* pchStart = _str.data();
	char* pch = NULL;
	while (true)
	{
		const char* pch = strchr(pchStart, split);

		if (pch)
			retVString.push_back(std::string(pchStart, pch - pchStart));
		else
			retVString.push_back(pchStart);

		if (!pch)
			break;

		pchStart = pch + 1;
	}
}

inline std::string getFileDirA(const char* filename)
{
	const char* lastSlash = strrchr(filename, ('/'));
	const char* lastBackSlash = strrchr(filename, ('\\'));
	if (!lastSlash || lastSlash < lastBackSlash)
		lastSlash = lastBackSlash;

	if (!lastSlash || lastSlash - filename <= 0)
		return std::string(".");
	else
	{
		return std::string(filename, size_t(lastSlash - filename));
	}
}

inline std::string getFileNameA(const char* filename)
{
	const char* lastSlash = strrchr(filename, '/');
	const char* lastBackSlash = strrchr(filename, '\\');
	if (lastSlash < lastBackSlash)
		lastSlash = lastBackSlash;

	bool noPrefix = (!lastSlash || *(lastSlash + 1) == '\0');
	if (noPrefix)
		return std::string(filename);
	else
		return std::string(lastSlash + 1);
}

inline std::string getFileNameNoExtensionA(const char* filename)
{
	const char* lastSlash = strrchr(filename, '/');
	const char* lastBackSlash = strrchr(filename, '\\');
	if (lastSlash < lastBackSlash)
		lastSlash = lastBackSlash;
	const char* p = strrchr(filename, '.');
	if (p && p < lastSlash)
	{
		return std::string(filename);
	}

	bool noPrefix = (!lastSlash || *(lastSlash + 1) == '\0');
	bool noSuffix = (!p || *(p + 1) == '\0');
	if (noPrefix && noSuffix)
	{
		return std::string(filename);
	}
	else if (noPrefix)			//suffix
	{
		return std::string(filename, size_t(p - filename));
	}
	else if (noSuffix)			//prefix
	{
		return std::string(lastSlash + 1);
	}
	else
	{
		return std::string(lastSlash + 1, size_t(p - (lastSlash + 1)));
	}
}