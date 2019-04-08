#include "CFileSystem.h"
#include "stringext.h"
#include "function.h"
#include "CWriteFile.h"
#include "CReadFile.h"

#define DATA_SUBDIR  "Data/"

CFileSystem::CFileSystem(const char * baseDir, const char* wowDir)
{
	BaseDirectory = baseDir;
	normalizeFileName(BaseDirectory);

	DataDirectory = BaseDirectory + DATA_SUBDIR;
	normalizeFileName(DataDirectory);

	WowBaseDirectory = wowDir;
	normalizeFileName(WowBaseDirectory);

	WowDataDirectory = WowBaseDirectory + DATA_SUBDIR;
	normalizeFileName(WowDataDirectory);
}

CFileSystem::~CFileSystem()
{

}

CReadFile * CFileSystem::createAndOpenFile(const char * filename, bool binary)
{
	CReadFile* file = new CReadFile(filename, binary);
	if (file->isOpen())
		return file;

	delete file;
	return nullptr;
}

CWriteFile* CFileSystem::createAndWriteFile(const char* filename, bool binary, bool append /*= false*/)
{
	CWriteFile* file = new CWriteFile(filename, binary, append);
	if (file->isOpen())
		return file;

	delete file;
	return nullptr;
}

bool CFileSystem::createDirectory(const char * dirname)
{
	int ret = Q_mkdir(dirname);

	if (ret == 0 || ret == EEXIST)
		return true;
	return false;
}

bool CFileSystem::deleteDirectory(const char * dirname)
{
	return Q_rmdir(dirname) == 0;
}

void CFileSystem::getAbsolutePath(const char* filename, char* outfilename, uint32_t size)
{
	Q_fullpath(filename, outfilename, size);
}

bool CFileSystem::isFileExists(const char* filename)
{
	if (strlen(filename) == 0)
		return false;
	return Q_access(filename, 0) == 0;
}
