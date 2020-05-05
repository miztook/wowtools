#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <map>
#include <array>
#include <functional>

#ifndef HANDLE
typedef void* HANDLE;
#endif

class CFileSystem;
class CMemFile;

using WOWFILECALLBACK = std::function<void(const char* filename)>;

class wowEnvironment
{
public:
	explicit wowEnvironment(CFileSystem* fs);
	~wowEnvironment();

public:
	struct SConfig
	{
		std::array<int, 4> version;
		std::string  locale;
		std::string  product;
	};

public:
	bool init();
	bool loadCascListFiles();

	CMemFile* openFile(const char* filename) const;
	CMemFile* openFileById(uint32_t fileid) const;
	bool exists(const char* filename) const;

	const CFileSystem* getFileSystem() const { return FileSystem; }
	const char* getLocale() const { return Config.locale.c_str(); }
	const std::array<int, 4>& getVersion() const { return Config.version; }
	const char* getProduct() const { return Config.product.c_str(); }

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
	bool initBuildInfo(SConfig& config);

	bool loadRoot();
	void unloadRoot();

private:
	CFileSystem*		FileSystem;
	uint32_t		CascLocale;
	SConfig			Config;
	HANDLE	hStorage;
	std::unordered_map<uint32_t, std::string>	FileId2NameMap;
	std::map<std::string, uint32_t>	FileName2IdMap;
	std::map<std::string, std::string>	DirIndexMap;

	//
	std::vector<std::string>		WmoFileList;
};

bool createWowEnvironment(CFileSystem* fs, bool loadCascFile);
void destroyWowEnvironment();

extern wowEnvironment* g_WowEnvironment;
