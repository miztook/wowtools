#include "wowEnvironment.h"
#include "CFileSystem.h"
#include "CMemFile.h"
#include "CReadFile.h"
#include "stringext.h"
#include "function.h"
#include <regex>

#include "CascLib.h"
#include "CascCommon.h"

#define LISTFILE "listfile.csv"

//
wowEnvironment* g_WowEnvironment = nullptr;
bool createWowEnvironment(CFileSystem * fs, bool loadCascFile)
{
	g_WowEnvironment = new wowEnvironment(fs);
	if (!g_WowEnvironment->init("wow_classic"))
	{
		delete g_WowEnvironment;
		g_WowEnvironment = nullptr;
		return false;
	}

	if (loadCascFile)
	{
		if (!g_WowEnvironment->loadCascListFiles())
			return false;
	}
	return true;
}

void destroyWowEnvironment()
{
	delete g_WowEnvironment;
	g_WowEnvironment = nullptr;
}

wowEnvironment::wowEnvironment(CFileSystem* fs)
	: FileSystem(fs), hStorage(nullptr)
{

}

wowEnvironment::~wowEnvironment()
{
	unloadRoot();
}

bool wowEnvironment::init(const char* product)
{
	std::vector<SConfig> configList;
	if (!initBuildInfo(configList))
		return false;

	auto itr = std::find_if(configList.begin(), configList.end(), [product](const SConfig& config) { return config.product == product; });
	if (itr == configList.end())
		return false;

	Config = *itr;
	const char* szRoot = FileSystem->getWowDataDirectory();
	if (!loadRoot(szRoot, Config))
		return false;

	return true;
}

bool wowEnvironment::loadCascListFiles()
{
	FileName2IdMap.clear();
	FileId2NameMap.clear();

	std::string dir = FileSystem->getDataDirectory();
	normalizeDirName(dir);
	dir += std_string_format("%d.%d", Config.version[0], Config.version[1]);
	normalizeDirName(dir);

	std::string listFile = dir + LISTFILE;

	CReadFile* file = FileSystem->createAndOpenFile(listFile.c_str(), false);
	if (!file)
		return false;

	char buffer[1024] = { 0 };
	while (file->readLine(buffer, 1024))
	{
		char* p = strstr(buffer, ";");
		if (!p)
			continue;

		std::string filename(p + 1);
		normalizeFileName(filename);
		str_tolower(filename);

		*p = '\0';

		int id = atoi(buffer);
		if (id >= 0)
		{
			FileName2IdMap[filename] = id;
			FileId2NameMap[id] = filename;
		}
	}

	delete file;

	DirIndexMap.clear();
	{
		for (const auto& itr : FileName2IdMap)
		{
			const char* szFile = itr.first.c_str();
			const char* p = strchr(szFile, '/');
			while (p)
			{
				std::string dir(szFile, (uint32_t)(p - szFile));
				if (DirIndexMap.find(dir) == DirIndexMap.end())
					DirIndexMap[dir] = szFile;														//add dir to index

				if (*(p + 1) == '0')
					break;
				p = strchr(p + 1, '/');
			}
		}
	}

	return true;
}

CMemFile * wowEnvironment::openFile(const char* filename) const
{
	HANDLE hFile;

	char realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	if (!CascOpenFile(hStorage, realfilename, Config.casclocale, 0, &hFile))
	{
		return nullptr;
	}

	DWORD dwHigh;
	uint32_t size = CascGetFileSize(hFile, &dwHigh);

	// HACK: in patch.mpq some files don't want to open and give 1 for filesize
	if (size <= 1 || size == 0xffffffff) {
		CascCloseFile(hFile);
		return nullptr;
	}

	uint8_t* buffer = new uint8_t[size];

	bool ret = CascReadFile(hFile, buffer, (DWORD)size, nullptr);
	if (!ret)
	{
		delete[] buffer;

		CascCloseFile(hFile);
		return nullptr;
	}

	CascCloseFile(hFile);

	return new CMemFile(buffer, size);
}

CMemFile* wowEnvironment::openFileById(uint32_t fileid) const
{
	HANDLE hFile;

	if (!CascOpenFile(hStorage, CASC_FILE_DATA_ID(fileid), Config.casclocale, CASC_OPEN_BY_FILEID, &hFile))
	{
		return nullptr;
	}

	DWORD dwHigh;
	uint32_t size = CascGetFileSize(hFile, &dwHigh);

	// HACK: in patch.mpq some files don't want to open and give 1 for filesize
	if (size <= 1 || size == 0xffffffff) {
		CascCloseFile(hFile);
		return nullptr;
	}

	uint8_t* buffer = new uint8_t[size];

	bool ret = CascReadFile(hFile, buffer, (DWORD)size, nullptr);
	if (!ret)
	{
		delete[] buffer;

		CascCloseFile(hFile);
		return nullptr;
	}

	CascCloseFile(hFile);

	return new CMemFile(buffer, size);
}

bool wowEnvironment::exists(const char * filename) const
{
	if (strlen(filename) == 0)
		return false;

	HANDLE hFile;
	if (!CascOpenFile(hStorage, filename, Config.casclocale, 0, &hFile))
		return false;

	CascCloseFile(hFile);
	return true;
}

void wowEnvironment::iterateFiles(const char* ext, WOWFILECALLBACK callback) const
{
	for (const auto& itr : FileName2IdMap)
	{
		const char* filename = itr.first.c_str();
		if (hasFileExtensionA(filename, ext))
		{
			callback(filename);
		}
	}
}

void wowEnvironment::iterateFiles(const char* path, const char * ext, WOWFILECALLBACK callback) const
{
	std::string strBaseDir(path);
	str_tolower(strBaseDir);
	auto itr = DirIndexMap.find(strBaseDir);
	if (itr == DirIndexMap.end())
		return;

	//calc start
	std::string firstFile = itr->second;

	auto it = FileName2IdMap.find(firstFile);
	while (it != FileName2IdMap.end())
	{
		const char* filename = itr->first.c_str();
		if (hasFileExtensionA(filename, ext))
		{
			callback(filename);
		}

		if (strstr(filename, strBaseDir.c_str()) == nullptr)
			break;

		++itr;
	}
}

const char* wowEnvironment::getFileNameById(uint32_t id) const
{
	auto itr = FileId2NameMap.find(id);
	if (itr != FileId2NameMap.end())
		return itr->second.c_str();
	return nullptr;
}

void wowEnvironment::buildWmoFileList()
{
	WmoFileList.clear();
	iterateFiles("world", "wmo", [this](const char* filename)
	{
		if (strstr(filename, "internal"))
			return;

		uint32_t len = (uint32_t)strlen(filename);
		if (len > 8 &&
			filename[len - 8] == '_' &&
			isdigit((int)filename[len - 7]) &&
			isdigit((int)filename[len - 6]) &&
			isdigit((int)filename[len - 5]))		//_xxx.wmo
		{
			return;
		}

		WmoFileList.emplace_back(filename);
	});
}

bool wowEnvironment::initBuildInfo(std::vector<SConfig>& configList)
{
	std::string buildInfo = FileSystem->getWowBaseDirectory();
	normalizeDirName(buildInfo);
	buildInfo += ".build.info";

	configList.clear();

	CReadFile* file = FileSystem->createAndOpenFile(buildInfo.c_str(), false);
	if (!file)
		return false;

	char buffer[1024] = { 0 };

	//read header
	file->readLine(buffer, 1024);
	std::vector<std::string> headers;
	int activeIndex = 0;
	int versionIndex = 0;
	int tagIndex = 0;
	int productIndex = 0;
	std_string_split(std::string(buffer), '|', headers);
	for (int i = 0; i < (int)headers.size(); ++i)
	{
		if (strstr(headers[i].c_str(), "Active"))
			activeIndex = i;
		else if (strstr(headers[i].c_str(), "Version"))
			versionIndex = i;
		else if (strstr(headers[i].c_str(), "Tags"))
			tagIndex = i;
		else if (strstr(headers[i].c_str(), "Product"))
			productIndex = i;
	}

	//read values
	while (file->readLine(buffer, 1024))
	{
		std::vector<std::string> values;
		std_string_split(std::string(buffer), '|', values, true);

		ASSERT(values.size() == headers.size());

		//skip inactive
		//if (values[activeIndex] == "0")
		//	continue;

		SConfig config;

		//version
		const std::regex pattern("(\\d+).(\\d+).(\\d+).(\\d+)");
		std::match_results<std::string::const_iterator> sm;
		std::regex_match(values[versionIndex], sm, pattern);
		if (sm.size() == 5)
		{
			config.version[0] = atoi(sm[1].str().c_str());
			config.version[1] = atoi(sm[2].str().c_str());
			config.version[2] = atoi(sm[3].str().c_str());
			config.version[3] = atoi(sm[4].str().c_str());
		}

		//product
		config.product = values[productIndex];

		//locale
		std::string tag = values[tagIndex];
		std_string_split(tag, ':', values);
		for (const auto& str : values)
		{
			if (strstr(str.c_str(), "text?"))
			{
				std::vector<std::string> taglist;
				std_string_split(str, ' ', taglist);

				if (taglist.size() >= 2)
				{
					config.locale = taglist[taglist.size() - 2];
					config.casclocale = getCascLocale(config.locale);
				}
			}
		}

		configList.push_back(config);
	}

	delete file;
	return true;
}

bool wowEnvironment::loadRoot(const char* szRootDir, const SConfig& config)
{
	unloadRoot();

	std::string strStorage = szRootDir;
	normalizeDirName(strStorage);
	rtrim(strStorage, "/");
	strStorage += ":";
	strStorage += config.product;

	if (!CascOpenStorage(strStorage.c_str(), config.casclocale, &hStorage))
	{
		hStorage = nullptr;
		return false;
	}
	return true;
}

void wowEnvironment::unloadRoot()
{
	if (hStorage != nullptr)
	{
		CascCloseStorage(hStorage);
		hStorage = nullptr;
	}
}

uint32_t wowEnvironment::getCascLocale(const std::string& locale) const
{
	if (locale == "enUS")
		return CASC_LOCALE_ENUS;
	else if (locale == "koKR")
		return CASC_LOCALE_KOKR;
	else if (locale == "frFR")
		return CASC_LOCALE_FRFR;
	else if (locale == "deDE")
		return CASC_LOCALE_DEDE;
	else if (locale == "zhCN")
		return CASC_LOCALE_ZHCN;
	else if (locale == "esES")
		return CASC_LOCALE_ESES;
	else if (locale == "ZhTW")
		return CASC_LOCALE_ZHTW;
	else if (locale == "enGB")
		return CASC_LOCALE_ENGB;
	else if (locale == "enCN")
		return CASC_LOCALE_ENCN;
	else if (locale == "enTW")
		return CASC_LOCALE_ENTW;
	else if (locale == "esMX")
		return CASC_LOCALE_ESMX;
	else if (locale == "ruRU")
		return CASC_LOCALE_RURU;
	else if (locale == "ptBR")
		return CASC_LOCALE_PTBR;
	else if (locale == "itIT")
		return CASC_LOCALE_ITIT;
	else if (locale == "ptPT")
		return CASC_LOCALE_PTPT;
	else
		return 0;
}
