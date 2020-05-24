#include "wowTable.h"

#include "wowDatabase.h"
#include "wowDbFile.h"

bool g_IterateTableRecords(const wowDatabase* database, const char* tableName, const std::function<void(const std::vector<VAR_T>&val)>& callback)
{
	const CTableStruct* table = database->getDBStruct(tableName);
	if (!table)
	{
		assert(false);
		return false;
	}

	const DBFile* file = database->loadDBFile(table->name.c_str());
	if (!file)
	{
		assert(false);
		return false;
	}

// 	if (table->name == "CreatureDisplayInfo")
// 	{
// 		int x = 0;
// 	}

	//read records
	for (uint32_t i = 0; i < file->getRecordCount(); ++i)
	{
		const std::vector<VAR_T>& val = file->getRecordValue(i, table);

		if (callback)
			callback(val);
	}

	delete file;
	return true;
}
