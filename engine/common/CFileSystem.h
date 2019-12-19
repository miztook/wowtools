#pragma once

#include "CSysSync.h"
#include <cstdint>
#include <string>
#include "CReadFile.h"
#include "CWriteFile.h"

enum E_LOG_TYPE : int
{
	ELOG_GX = 0,
	ELOG_RES,
};

class CFileSystem
{
public:
	explicit CFileSystem(const char* wowDir);
	~CFileSystem();

public:
	 CReadFile* createAndOpenFile(const char* filename, bool binary);
	 CWriteFile* createAndWriteFile(const char* filename, bool binary, bool append = false);

	 bool createDirectory(const char* dirname);
	 bool deleteDirectory(const char* dirname);

	 void getAbsolutePath(const char* filename, char* outfilename, uint32_t size);

	 const char* getWorkingDirectory() const { return WorkingDirectory.c_str(); }
	 const char* getDataDirectory() const { return DataDirectory.c_str(); }
	 const char* getWowBaseDirectory() const { return WowBaseDirectory.c_str(); }
	 const char* getWowDataDirectory() const { return WowDataDirectory.c_str(); }
	 
	 void writeLog(E_LOG_TYPE type, const char* format, ...);

	 bool isFileExists(const char* filename) const;
	 uint32_t getFileSize(const char* filename) const;
	 bool changeFileMode(const char* filename, int mode) const;
	 bool deleteFile(const char* filename) const;
	 bool copyFile(const char* src, const char* des) const;
	 bool moveFile(const char* src, const char* des) const;

private:
	bool createLogFiles();

private:
	std::string		WorkingDirectory;
	std::string		DataDirectory;
	std::string		LogDirectory;
	std::string		WowBaseDirectory;
	std::string		WowDataDirectory;
	
	CWriteFile*		LogFile;
	char	LogString[2048];

	lock_type	LogCS;
};

bool createFileSystem(const char* wowDir);
void destroyFileSystem();

extern CFileSystem* g_FileSystem;