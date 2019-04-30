#pragma once

#include <stdint.h>
#include <vector>
#include "varianttype.h"
#include "wowEnums.h"
#include "CMemFile.h"

class CTableStruct;

using VAR_T = Variant<uint32_t, uint64_t, uint16_t, int, float, std::string>;

class DBFile
{
public:
	explicit DBFile(CMemFile* memFile)
		: m_pMemFile(memFile)
		, recordSize(0), recordCount(0), fieldCount(0), stringSize(0)
		, data(nullptr), stringTable(nullptr)
	{

	}
	virtual ~DBFile()
	{
		delete m_pMemFile;
	}

public:
	static const DBFile* readDBFile(CMemFile* memFile);

public:
	virtual std::vector<VAR_T> getRecordValue(uint32_t index, const CTableStruct* table) const = 0;

	uint32_t getRecordCount() const { return recordCount; }

protected:
	CMemFile* m_pMemFile;

	uint32_t recordSize;
	uint32_t recordCount;
	uint32_t fieldCount;
	uint32_t stringSize;
	const uint8_t* data;
	const uint8_t* stringTable;
};