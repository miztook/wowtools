#pragma once

#include <cstdint>
#include <string>

class CReadFile;
class CWriteFile;

class CFileSystem
{
public:
	CFileSystem(const char* baseDir, const char* wowDir);
	~CFileSystem();

public:
	 CReadFile* createAndOpenFile(const char* filename, bool binary);
	 CWriteFile* createAndWriteFile(const char* filename, bool binary, bool append = false);

	 bool createDirectory(const char* dirname);
	 bool deleteDirectory(const char* dirname);

	 void getAbsolutePath(const char* filename, char* outfilename, uint32_t size);

	 const char* getBaseDirectory() const { return BaseDirectory.c_str(); }
	 const char* getDataDirectory() const { return DataDirectory.c_str(); }
	 const char* getWowBaseDirectory() const { return WowBaseDirectory.c_str(); }
	 const char* getWowDataDirectory() const { return WowDataDirectory.c_str(); }
	 bool isFileExists(const char* filename);

private:
	std::string		BaseDirectory;
	std::string		DataDirectory;
	std::string		WowBaseDirectory;
	std::string		WowDataDirectory;
};