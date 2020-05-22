#pragma once

#include "wowDbFile.h"
#include <vector>
#include <map>

class WDC3File : public DBFile
{
public:
	struct header
	{
		char   magic[4];               // 'WDC3'
		uint32_t record_count;           // this is for all sections combined now
		uint32_t field_count;
		uint32_t record_size;
		uint32_t string_table_size;      // this is for all sections combined now
		uint32_t table_hash;             // hash of the table name
		uint32_t layout_hash;            // this is a hash field that changes only when the structure of the data changes
		uint32_t min_id;
		uint32_t max_id;
		uint32_t locale;                 // as seen in TextWowEnum
		uint16_t flags;                  // possible values are listed in Known Flag Meanings
		uint16_t id_index;               // this is the index of the field containing ID values; this is ignored if flags & 0x04 != 0
		uint32_t total_field_count;      // from WDC1 onwards, this value seems to always be the same as the 'field_count' value
		uint32_t bitpacked_data_offset;  // relative position in record where bitpacked data begins; not important for parsing the file
		uint32_t lookup_column_count;
		uint32_t field_storage_info_size;
		uint32_t common_data_size;
		uint32_t pallet_data_size;
		uint32_t section_count;          // new to WDC3, this is number of sections of data (records + copy table + id list + relationship map = a section)
	};

	struct section_header
	{
		uint64_t tact_key_hash;          // TactKeyLookup hash
		uint32_t file_offset;            // Absolute position to the beginning of the section
		uint32_t record_count;           // 'record_count' for the section
		uint32_t string_table_size;      // 'string_table_size' for the section
		uint32_t offset_records_end;     // Offset to the spot where the records end in a file with an offset map structure;
		uint32_t id_list_size;           // Size of the list of ids present in the section
		uint32_t relationship_data_size; // Size of the relationship data in the section
		uint32_t offset_map_id_count;    // Count of ids present in the offset map in the section
		uint32_t copy_table_count;       // Count of the number of deduplication entries (you can multiply by 8 to mimic the old 'copy_table_size' field
	};

	explicit WDC3File(CMemFile* memFile);
	~WDC3File() = default;

	bool open();

	std::vector<VAR_T> getRecordValue(uint32_t index, const CTableStruct* table) const override;

private:
	enum class FIELD_COMPRESSION : uint32_t
	{
		NONE = 0,
		BITPACKED,
		COMMON_DATA,
		BITPACKED_INDEXED,
		BITPACKED_INDEXED_ARRAY,
		BITPACKED_SIGNED,
	};

	struct field_storage_info
	{
		uint16_t field_offset_bits;
		uint16_t field_size_bits; // very important for reading bitpacked fields; size is the sum of all array pieces in bits - for example, uint32[3] will appear here as '96'
								  // additional_data_size is the size in bytes of the corresponding section in
								  // common_data or pallet_data.  These sections are in the same order as the
								  // field_info, so to find the offset, add up the additional_data_size of any
								  // previous fields which are stored in the same block (common_data or
								  // pallet_data).
		uint32_t additional_data_size;
		FIELD_COMPRESSION storage_type;
		uint32_t val1;
		uint32_t val2;
		uint32_t val3;
	};

#pragma pack(2)
	struct offset_map_entry
	{
		uint32_t offset;
		uint16_t size;
	};
#pragma pack()

	struct relationship_entry 
	{
		uint32_t foreign_id;
		uint32_t record_index;
	};

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

	bool readFieldValue(uint32_t recordIndex, uint32_t fieldIndex, uint32_t arrayIndex, uint32_t arraySize, uint32_t& result) const;
	uint32_t readBitpackedValue(const field_storage_info& info, const uint8_t* recordOffset) const;

private:
	std::vector<uint32_t> m_IDs;
	std::map<int, int> m_fieldSizes;
	std::vector<const uint8_t*> m_recordOffsets;

	bool m_isSparseTable;

	WDC3File::header m_header;
	std::vector<section_header> m_sectionHeaders;
	std::vector<field_storage_info> m_fieldStorageInfo;

	std::map<uint32_t, uint32_t> m_palletBlockOffsets;
	std::map<uint32_t, std::map<uint32_t, uint32_t> > m_commonData;
	std::map<uint32_t, uint32_t> m_relationShipData;

	std::vector<uint8_t> m_palletData;
};
