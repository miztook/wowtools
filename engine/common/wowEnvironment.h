#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>
#include <array>
#include <functional>
#include "stringext.h"

#ifndef HANDLE
typedef void* HANDLE;
#endif

class CFileSystem;
class CMemFile;

using WOWFILECALLBACK = std::function<void(const char* filename)>;
using WOWFILEIDCALLBACK = std::function<void(uint32_t fileDataId)>;

class wowEnvironment
{
public:
	explicit wowEnvironment(CFileSystem* fs);
	~wowEnvironment();

public:
	struct SConfig
	{
		SConfig()
		{
			version[0] = version[1] = version[2] = version[3] = 0;
			casclocale = 0;
			versionString = std_string_format("%d.%d.%d.%d", version[0], version[1], version[2], version[3]);
		}
		std::array<int, 4> version;
		std::string  locale;
		std::string  product;
		std::string  versionString;
		uint32_t	casclocale;
	};

public:
	bool init(const char* product);
	bool loadCascListFiles();

	CMemFile* openFile(const char* filename) const;
	CMemFile* openFileById(uint32_t fileid) const;
	bool exists(const char* filename) const;

	const CFileSystem* getFileSystem() const { return FileSystem; }
	const char* getLocale() const { return Config.locale.c_str(); }
	const std::array<int, 4>& getVersion() const { return Config.version; }
	const char* getProduct() const { return Config.product.c_str(); }
	const char* getVersionString() const { return Config.versionString.c_str(); }

	//
	void iterateFiles(const char* ext, WOWFILECALLBACK callback) const;
	void iterateFiles(const char* path, const char* ext, WOWFILECALLBACK callback) const;
	void iterateAllFileId(WOWFILEIDCALLBACK callback) const;

	const char* getFileNameById(uint32_t id) const;
	uint32_t getFileIdByName(const char* filename) const;

public:
	void buildWmoFileList();

public:
	uint32_t getNumWmos() const { return (uint32_t)WmoFileList.size(); }
	const char* getWmoFileName(uint32_t index) const { return WmoFileList[index].c_str(); }

private:
	bool initBuildInfo(std::vector<SConfig>& configList);

	bool loadRoot(const char* szRootDir, const SConfig& config);
	void unloadRoot();

	uint32_t getCascLocale(const std::string& locale) const;

private:
	CFileSystem*		FileSystem;
	SConfig			Config;
	HANDLE	hStorage;
	std::map<uint32_t, std::string>	FileId2NameMap;
	std::map<std::string, uint32_t>	FileName2IdMap;
	std::map<std::string, std::string>	DirIndexMap;

	//
	std::vector<std::string>		WmoFileList;
};

bool createWowEnvironment(CFileSystem* fs, bool loadCascFile);
void destroyWowEnvironment();

extern wowEnvironment* g_WowEnvironment;
