#include "wowTable.h"

#include "wowDatabase.h"
#include "wowDbFile.h"

bool BaseTable::loadData(const wowDatabase * database, const char* tableName)
{
	const CTableStruct* table = database->getDBStruct(tableName);
	if (!table)
	{
		assert(false);
		return false;
	}

	const DBFile* file = database->loadDBFile(table);
	if (!file)
	{
		assert(false);
		return false;
	}

	//read records
	for (uint32_t i = 0; i < file->getRecordCount(); ++i)
	{
		const std::vector<VAR_T>& val = file->getRecordValue(i, table);
		
		if (onLoadItem)
			onLoadItem(val);
	}

	delete file;
	return true;
}


