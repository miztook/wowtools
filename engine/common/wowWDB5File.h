#pragma once

#include "wowDbFile.h"
#include <vector>
#include <map>

class WDB5File : public DBFile
{
public:

	struct header
	{
		char magic[4];                                               // 'WDB5' for .db2 (database)
		uint32_t record_count;
		uint32_t field_count;                                         // for the first time, this counts arrays as '1'; in the past, only the WCH* variants have counted arrays as 1 field
		uint32_t record_size;
		uint32_t string_table_size;                                   // if flags & 0x01 != 0, this field takes on a new meaning - it becomes an absolute offset to the beginning of the offset_map
		uint32_t table_hash;
		uint32_t layout_hash;                                         // used to be 'build', but after build 21737, this is a new hash field that changes only when the structure of the data changes
		uint32_t min_id;
		uint32_t max_id;
		uint32_t locale;                                              // as seen in TextWowEnum
		uint32_t copy_table_size;
		uint16_t flags;                                               // in WDB3/WCH4, this field was in the WoW executable's DBCMeta; possible values are listed in Known Flag Meanings
		uint16_t id_index;                                            // new in WDB5 (and only after build 21737), this is the index of the field containing ID values; this is ignored if flags & 0x04 != 0
	};

	WDB5File(CMemFile* memFile);
	~WDB5File() = default;

	bool open();

	virtual std::vector<VAR_T> getRecordValue(uint32_t index, const CTableStruct* table) const override;

protected:
	struct field_structure
	{
		int16_t size;
		uint16_t position;
	};

	struct copy_table_entry
	{
		uint32_t newRowId;
		uint32_t copiedRowId;
	};

	std::vector<uint32_t> m_IDs;
	std::map<int, int> m_fieldSizes;
	std::vector<const uint8_t*> m_recordOffsets;

	bool m_isSparseTable;

};