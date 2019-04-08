#pragma once

#include <string>
#include <vector>
#include <set>

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
	bool init(const char* szLocale);

	const CMemFile* openFile(const char* filename) const;
	bool exists(const char* filename) const;

	const char* getLocale() const { return Locale.c_str(); }

private:
	void initBuildInfo();

	bool loadRoot();
	void unloadRoot();

	void loadCascListFiles();

private:
	CFileSystem*		FileSystem;
	std::string		Locale;
	uint32_t		CascLocale;
	HANDLE	hStorage;
};