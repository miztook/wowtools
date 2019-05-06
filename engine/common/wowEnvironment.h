#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>
#include <array>
#include <functional>

#include "CascLib.h"
#include "CascCommon.h"

class CFileSystem;
class CMemFile;

using WOWFILECALLBACK = std::function<void(const char* filename)>;

class wowEnvironment
{
public:
	explicit wowEnvironment(CFileSystem* fs);
	~wowEnvironment();

public:
	bool init();
	bool loadCascListFiles();

	CMemFile* openFile(const char* filename) const;
	bool exists(const char* filename) const;

	const CFileSystem* getFileSystem() const { return FileSystem; }
	const char* getLocale() const { return Locale.c_str(); }
	const std::array<int, 4>& getVersion() const { return Version; }

	//
	void iterateFiles(const char* ext, WOWFILECALLBACK callback) const;
	void iterateFiles(const char* path, const char* ext, WOWFILECALLBACK callback) const;

	const char* getFileNameById(uint32_t id) const;

public:
	void buildWmoFileList();

public:
	uint32_t getNumWmos() const { return (uint32_t)WmoFileList.size(); }
	const char* getWmoFileName(uint32_t index) const { return WmoFileList[index].c_str(); }

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
	std::unordered_map<uint32_t, uint32_t>	FileIdMap;
	std::vector<std::string>	CascListFiles;
	std::map<std::string, int>	DirIndexMap;

	//
	std::vector<std::string>		WmoFileList;
};