#include "CFileSystem.h"
#include "stringext.h"
#include "function.h"
#include "CWriteFile.h"
#include "CReadFile.h"

#define DATA_SUBDIR  "Data/"
#define LOG_SUBDIR	"Logs/"

CFileSystem* g_FileSystem = nullptr;

bool createFileSystem(const char* wowDir)
{
	g_FileSystem = new CFileSystem(wowDir);

	return true;
}

void destroyFileSystem()
{
	delete g_FileSystem;
	g_FileSystem = nullptr;
}

CFileSystem::CFileSystem(const char* wowDir)
{
	char workingDir[QMAX_PATH];
	Q_getcwd(workingDir, QMAX_PATH);
	WorkingDirectory = workingDir;
	normalizeDirName(WorkingDirectory);

	DataDirectory = WorkingDirectory + DATA_SUBDIR;
	normalizeDirName(DataDirectory);

	LogDirectory = WorkingDirectory + LOG_SUBDIR;
	normalizeDirName(LogDirectory);

	WowBaseDirectory = wowDir;
	normalizeDirName(WowBaseDirectory);

	WowDataDirectory = WowBaseDirectory + DATA_SUBDIR;
	normalizeDirName(WowDataDirectory);

	createLogFiles();

	INIT_LOCK(&LogCS);
}

CFileSystem::~CFileSystem()
{
	DESTROY_LOCK(&LogCS);

	delete LogFile;
}

CReadFile * CFileSystem::createAndOpenFile(const char* filename, bool binary)
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

bool CFileSystem::createDirectory(const char * dirname) const
{
	int ret = Q_mkdir(dirname);

	if (ret == 0 || ret == EEXIST)
		return true;
	return false;
}

bool CFileSystem::deleteDirectory(const char * dirname) const
{
	return Q_rmdir(dirname) == 0;
}

void CFileSystem::getAbsolutePath(const char* filename, char* outfilename, uint32_t size) const
{
	Q_fullpath(filename, outfilename, size);
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

bool CFileSystem::copyFile(const char* src, const char* des) const
{
	const int BUF_SIZE = 1024;
	FILE* fromfd = NULL;
	FILE*  tofd = NULL;
	size_t bytes_read = 0;
	size_t bytes_write = 0;
	char buffer[BUF_SIZE];

	/*open source file*/
	if ((fromfd = fopen(src, "r")) == NULL)
	{
		//fprintf(stderr,"Open source file failed:%s\n",strerror(errno));
		return false;
	}
	/*create dest file*/
	if ((tofd = fopen(des, "wb")) == NULL)
	{
		//fprintf(stderr,"Create dest file failed:%s\n",strerror(errno));
		fclose(fromfd);
		return false;
	}

	/*copy file code*/
	while ((bytes_read = fread(buffer, 1, BUF_SIZE, fromfd)))
	{
		if (bytes_read == -1 && errno != EINTR)
			break; /*an important mistake occured*/
		else if (bytes_read == 0)
		{
			break;
		}
		else if (bytes_read > 0)
		{
			bytes_write = fwrite(buffer, 1, bytes_read, tofd);
			ASSERT(bytes_write == bytes_read);
		}
	}
	fclose(fromfd);
	fclose(tofd);
	return true;
}

bool CFileSystem::moveFile(const char* src, const char* des) const
{
	if (!copyFile(src, des))
		return false;
	if (!deleteFile(src))
		return false;
	return  true;
}

void CFileSystem::makeDirectory(const char* dir) const
{
	makeDirectory(dir, (int)strlen(dir));
}

void CFileSystem::makeDirectory(const char* dir, int r) const
{
	--r;
	while (r > 0 && dir[r] != '/'&&dir[r] != '\\')
		--r;
	if (r == 0)
		return;
	makeDirectory(dir, r);
	char t[400];
	strcpy(t, dir);
	t[r] = '\0';
	createDirectory(t);
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
	if (LogFile)
	{
		LogFile->writeText(text.c_str());
		LogFile->flush();
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
		path.append("gamelog.txt");
		LogFile = createAndWriteFile(path.c_str(), false, false);
	}

	return true;
}

