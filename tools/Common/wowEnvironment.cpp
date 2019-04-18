#include "wowEnvironment.h"
#include "CFileSystem.h"
#include "CMemFile.h"
#include "CReadFile.h"
#include "stringext.h"
#include "function.h"
#include <regex>

#define LISTFILE "listfile.txt"

wowEnvironment::wowEnvironment(CFileSystem* fs)
	: FileSystem(fs), hStorage(nullptr), CascLocale(0)
{

}

wowEnvironment::~wowEnvironment()
{
	unloadRoot();
}

bool wowEnvironment::init()
{
	if (!initBuildInfo(Locale))
		return false;

	if (Locale == "enUS")
		CascLocale = CASC_LOCALE_ENUS;
	else if (Locale == "koKR")
		CascLocale = CASC_LOCALE_KOKR;
	else if (Locale == "frFR")
		CascLocale = CASC_LOCALE_FRFR;
	else if (Locale == "deDE")
		CascLocale = CASC_LOCALE_DEDE;
	else if (Locale == "zhCN")
		CascLocale = CASC_LOCALE_ZHCN;
	else if (Locale == "esES")
		CascLocale = CASC_LOCALE_ESES;
	else if (Locale == "ZhTW")
		CascLocale = CASC_LOCALE_ZHTW;
	else if (Locale == "enGB")
		CascLocale = CASC_LOCALE_ENGB;
	else if (Locale == "enCN")
		CascLocale = CASC_LOCALE_ENCN;
	else if (Locale == "enTW")
		CascLocale = CASC_LOCALE_ENTW;
	else if (Locale == "esMX")
		CascLocale = CASC_LOCALE_ESMX;
	else if (Locale == "ruRU")
		CascLocale = CASC_LOCALE_RURU;
	else if (Locale == "ptBR")
		CascLocale = CASC_LOCALE_PTBR;
	else if (Locale == "itIT")
		CascLocale = CASC_LOCALE_ITIT;
	else if (Locale == "ptPT")
		CascLocale = CASC_LOCALE_PTPT;
	else
		CascLocale = 0;

	loadRoot();

	return true;
}

bool wowEnvironment::loadCascListFiles()
{
	CascListFiles.clear();
	FileIdMap.clear();

	std::string dir = FileSystem->getDataDirectory();
	normalizeDirName(dir);
	dir += std_string_format("%d.%d", Version[0], Version[1]);
	normalizeDirName(dir);

	std::string listFile = dir + LISTFILE;

	CReadFile* file = FileSystem->createAndOpenFile(listFile.c_str(), false);
	if (!file)
		return false;

	FileIdMap.reserve(900000);
	char buffer[1024] = { 0 };
	while (file->readLine(buffer, 1024))
	{
		std::string filename(buffer);

		if (filename.length() == 0)
			break;
		//normalizeFileName(filename);

		int id = (int)CascGetFileId(hStorage, filename.c_str());
		if (id >= 0)
		{
			CascListFiles.emplace_back(filename);
			FileIdMap[id] = (int)CascListFiles.size() - 1;
		}
	}

	delete file;
	return true;
}

bool wowEnvironment::initBuildInfo(std::string& activeLocale)
{
	std::string buildInfo = FileSystem->getWowBaseDirectory();
	normalizeDirName(buildInfo);
	buildInfo += ".build.info";

	CReadFile* file = FileSystem->createAndOpenFile(buildInfo.c_str(), false);
	if (!file)
		return false;

	activeLocale = "";
	char buffer[1024] = { 0 };

	//read header
	file->readLine(buffer, 1024);
	std::vector<std::string> headers;
	int activeIndex = 0;
	int versionIndex = 0;
	int tagIndex = 0;
	std_string_split(std::string(buffer), '|', headers);
	for (int i = 0; i < (int)headers.size(); ++i)
	{
		if (strstr(headers[i].c_str(), "Active"))
			activeIndex = i;
		else if (strstr(headers[i].c_str(), "Version"))
			versionIndex = i;
		else if (strstr(headers[i].c_str(), "Tags"))
			tagIndex = i;
	}

	//read values
	while (file->readLine(buffer, 1024))
	{
		std::vector<std::string> values;
		std_string_split(std::string(buffer), '|', values);

		assert(values.size() == headers.size());

		//skip inactive
		if (values[activeIndex] == "0")
			continue;

		//version
		const std::regex pattern("^(\\d).(\\d).(\\d).(\\d+)");
		std::match_results<std::string::const_iterator> sm;
		std::regex_match(values[versionIndex], sm, pattern);
		if (sm.size() == 5)
		{
			Version[0] = atoi(sm[1].str().c_str());
			Version[1] = atoi(sm[2].str().c_str());
			Version[2] = atoi(sm[3].str().c_str());
			Version[3] = atoi(sm[4].str().c_str());
		}

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
					activeLocale = taglist[taglist.size() - 2];
			}
		}
	}

	delete file;
	return true;
}

bool wowEnvironment::loadRoot()
{
	unloadRoot();
	const char* dataDir = FileSystem->getWowDataDirectory();
	if (!CascOpenStorage(dataDir, CascLocale, &hStorage))
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
