#include "CFileSystem.h"
#include "stringext.h"
#include "function.h"
#include "CWriteFile.h"
#include "CReadFile.h"

#define DATA_SUBDIR  "Data/"
#define LOG_SUBDIR	"Logs/"

CFileSystem* g_FileSystem = nullptr;

bool createFileSystem(const char* baseDir, const char* wowDir)
{
	g_FileSystem = new CFileSystem(baseDir, wowDir);

	return true;
}

void destroyFileSystem()
{
	delete g_FileSystem;
	g_FileSystem = nullptr;
}

CFileSystem::CFileSystem(const char * baseDir, const char* wowDir)
{
	BaseDirectory = baseDir;
	normalizeDirName(BaseDirectory);

	DataDirectory = BaseDirectory + DATA_SUBDIR;
	normalizeDirName(DataDirectory);

	WowBaseDirectory = wowDir;
	normalizeDirName(WowBaseDirectory);

	WowDataDirectory = WowBaseDirectory + DATA_SUBDIR;
	normalizeDirName(WowDataDirectory);

	LogDirectory = BaseDirectory + LOG_SUBDIR;
	normalizeDirName(LogDirectory);

	createLogFiles();

	INIT_LOCK(&LogCS);
}

CFileSystem::~CFileSystem()
{
	DESTROY_LOCK(&LogCS);

	delete LogResFile;
	delete LogGxFile;
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

std::string CFileSystem::getWorkingDirectory()
{
	char workingDirectory[QMAX_PATH];
	Q_getcwd(workingDirectory, QMAX_PATH);

	std::string ret = workingDirectory;
	return ret;
}

bool CFileSystem::isFileExists(const char* filename) const
{
	if (strlen(filename) == 0)
		return false;
	return Q_access(filename, 0) == 0;
}

uint32_t CFileSystem::getFileSize(const char* filename) const
{
	struct stat fileStat;
	stat(filename, &fileStat);
	return (uint32_t)(fileStat.st_size);
}

bool CFileSystem::changeFileMode(const char* filename, int mode) const
{
	return chmod(filename, mode) != -1;
}

bool CFileSystem::deleteFile(const char* filename) const
{
	return remove(filename) != -1;
}

void CFileSystem::writeLog(E_LOG_TYPE type, const char* format, ...)
{
	CLock lock(LogCS);

	va_list va;
	va_start(va, format);
	Q_vsprintf(LogString, 2048, format, va);
	va_end(va);

	std::string content = LogString;
	content.append("\n");

#ifdef A_PLATFORM_WIN_DESKTOP
	OutputDebugStringA(content.c_str());
#endif

	char timebuf[64];
	Q_getLocalTime(timebuf, 64);

	std::string text = timebuf;
	text.append(content);
	switch (type)
	{
	case ELOG_GX:
		if (LogGxFile)
		{
			LogGxFile->writeText(text.c_str());
			LogGxFile->flush();
		}
		break;
	case ELOG_RES:
		if (LogResFile)
		{
			LogResFile->writeText(text.c_str());
			LogResFile->flush();
		}
		break;
	default:
		break;
	}
}

bool CFileSystem::createLogFiles()
{
	if (!isFileExists(LogDirectory.c_str()))
	{
		if (!createDirectory(LogDirectory.c_str()))
		{
			ASSERT(false);
			return false;
		}
	}

	{
		std::string  path = LogDirectory;
		normalizeDirName(path);
		path.append(getLogFileName(ELOG_RES));
		LogResFile = createAndWriteFile(path.c_str(), false, false);
	}
	//gx log
	{
		std::string path = LogDirectory;
		normalizeDirName(path);
		path.append(getLogFileName(ELOG_GX));
		LogGxFile = createAndWriteFile(path.c_str(), false, false);
	}

	return true;
}

const char* CFileSystem::getLogFileName(E_LOG_TYPE type) const
{
	switch (type)
	{
	case ELOG_GX:
		return "gx.log";
	case ELOG_RES:
		return "resource.log";
	default:
		return "";
	}
}
