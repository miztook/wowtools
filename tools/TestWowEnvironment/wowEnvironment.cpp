#include "wowEnvironment.h"
#include "CFileSystem.h"
#include "CMemFile.h"
#include "CReadFile.h"
#include "stringext.h"

wowEnvironment::wowEnvironment(CFileSystem* fs)
	: FileSystem(fs)
{

}

wowEnvironment::~wowEnvironment()
{
	unloadRoot();
}

bool wowEnvironment::init(const char * szLocale)
{
	Locale = szLocale;
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

	return false;
}

void wowEnvironment::initBuildInfo()
{
	std::string buildInfo = FileSystem->getBaseDirectory();
	normalizeFileName(buildInfo);
	buildInfo += ".build.info";

	CReadFile* file = FileSystem->createAndOpenFile(buildInfo.c_str(), false);
	
	delete file;
}

bool wowEnvironment::loadRoot()
{
	unloadRoot();
	const char* dataDir = FileSystem->getDataDirectory();
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
