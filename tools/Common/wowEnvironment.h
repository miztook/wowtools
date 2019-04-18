#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <array>

#include "CascLib.h"
#include "CascCommon.h"

class CFileSystem;
class CMemFile;

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

	const CFileSystem* getFileSystem() const { return FileSystem; }

	const char* getLocale() const { return Locale.c_str(); }
	const std::array<int, 4>& getVersion() const { return Version; }

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
	std::unordered_map<int, int>	FileIdMap;
	std::vector<std::string> CascListFiles;
};