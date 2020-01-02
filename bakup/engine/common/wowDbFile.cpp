#include "wowDbFile.h"

#include "CMemFile.h"
#include "wowWDB5File.h"
#include "wowWDC3File.h"
#include <cassert>

const DBFile* DBFile::readDBFile(CMemFile * memFile)
{
	const char* magic = (const char*)memFile->getBuffer();

	WowDBType dbType = WowDBType::Unknown;
	if (strncmp(magic, "WDBC", 4) == 0)
		dbType = WowDBType::WDBC;
	else if (strncmp(magic, "WDB2", 4) == 0)
		dbType = WowDBType::WDB2;
	else if (strncmp(magic, "WDB5", 4) == 0)
		dbType = WowDBType::WDB5;
	else if (strncmp(magic, "WDB6", 4) == 0)
		dbType = WowDBType::WDB6;
	else if (strncmp(magic, "WDC1", 4) == 0)
		dbType = WowDBType::WDC1;
	else if (strncmp(magic, "WDC2", 4) == 0)
		dbType = WowDBType::WDC2;
	else if (strncmp(magic, "WDC3", 4) == 0)
		dbType = WowDBType::WDC3;

	if (dbType == WowDBType::WDC3)
	{
		WDC3File* file = new WDC3File(memFile);
		if (!file->open())
		{
			delete file;
			return nullptr;
		}
		else
			return file;
	}
	else
	{
		assert(false);
	}

	return nullptr;
}
