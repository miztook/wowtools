#pragma once

#include <stdint.h>
#include <vector>
#include "varianttype.h"
#include "wowEnums.h"

class CTableStruct;
class CMemFile;

using VAR_T = Variant<uint32_t, uint8_t, uint64_t, int32_t, std::string>;

class DBFile
{
public:
	DBFile()
		: recordSize(0), recordCount(0), fieldCount(0), stringSize(0)
		, data(nullptr), stringTable(nullptr)
	{

	}
	virtual ~DBFile() {}

public:
	static const DBFile* readDBFile(CMemFile* memFile);

public:
	virtual bool open() = 0;
	virtual void close() = 0;
	virtual std::vector<VAR_T> getRecordValue(uint32_t index, const CTableStruct* table) = 0;

	uint32_t getRecordCount() const { return recordCount; }

protected:
	uint32_t recordSize;
	uint32_t recordCount;
	uint32_t fieldCount;
	uint32_t stringSize;
	uint8_t* data;
	uint8_t* stringTable;
};