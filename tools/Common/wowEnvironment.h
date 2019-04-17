#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <array>

#include "CascLib.h"
#include "CascCommon.h"

class CFileSystem;
class CMemFile;

#define LISTFILE    "listfile80.txt"

class wowEnvironment
{
public:
	explicit wowEnvironment(CFileSystem* fs);
	~wowEnvironment();

public:
	bool init();
	bool loadCascListFiles();

	const CMemFile* openFile(const char* filename) const;
	bool exists(const char* filename) const;

	const char* getLocale() const { return Locale.c_str(); }

private:
	bool initBuildInfo(std::string& activeLocale);

	bool loadRoot();
	void unloadRoot();

private:
	CFileSystem*		FileSystem;
	uint32_t		CascLocale;
	std::string		Locale;
	std::array<int, 4>			Version;
	HANDLE	hStorage;
	std::map<int, int>	FileIdMap;
	std::vector<std::string> CascListFiles;
};