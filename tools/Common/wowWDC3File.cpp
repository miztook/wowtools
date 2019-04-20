#include "wowWDC3File.h"

#include "CMemFile.h"
#include "wowDatabase.h"
#include "stringext.h"
#include <cassert>

bool WDC3File::open(CMemFile * memFile)
{
	const uint8_t* buffer = memFile->getBuffer();

	WDC3File::header header;
	memFile->read(&header, sizeof(header));

	recordSize = header.record_size;
	recordCount = header.record_count;
	fieldCount = header.field_count;
	stringSize = header.string_table_size;

	//section header
	m_sectionHeaders.resize(header.section_count);
	memFile->read(m_sectionHeaders.data(), sizeof(section_header) * header.section_count);

	//field
	std::vector<WDB5File::field_structure> fields;
	fields.resize(fieldCount);
	memFile->read(fields.data(), fieldCount * sizeof(WDB5File::field_structure));
	for (const auto& field : fields)
	{
		m_fieldSizes[field.position] = field.size;
	}

	//storage info
	if (header.field_storage_info_size > 0)
	{
		uint32_t nFieldStorageInfo = header.field_storage_info_size / sizeof(field_storage_info);
		m_fieldStorageInfo.resize(nFieldStorageInfo);
		memFile->read(m_fieldStorageInfo.data(), sizeof(field_storage_info) * nFieldStorageInfo);
	}

	//pallte data
	if (header.pallet_data_size > 0)
	{
		m_palletData.resize(header.pallet_data_size);
		memFile->read(m_palletData.data(), header.pallet_data_size);

		uint32_t fieldId = 0;
		uint32_t offset = 0;
		for (const auto& info : m_fieldStorageInfo)
		{
			if ((info.storage_type == FIELD_COMPRESSION::BITPACKED_INDEXED || info.storage_type == FIELD_COMPRESSION::BITPACKED_INDEXED_ARRAY) &&
				info.additional_data_size != 0)
			{
				m_palletBlockOffsets[fieldId] = offset;
				offset += info.additional_data_size;
			}
			++fieldId;
		}
	}

	//common data
	if (header.common_data_size > 0)
	{
		const uint8_t* commonData = memFile->getPointer();

		uint32_t fieldId = 0;
		uint32_t offset = 0;
		for (const auto& info : m_fieldStorageInfo)
		{
			if ((info.storage_type == FIELD_COMPRESSION::COMMON_DATA) &&
				info.additional_data_size != 0)
			{
				const uint8_t* ptr = commonData + offset;
				std::map<uint32_t, uint32_t> commonVals;
				for (uint32_t i = 0; i < info.additional_data_size / 8; ++i)
				{
					uint32_t id;
					uint32_t val;
					memcpy(&id, ptr, 4);
					ptr += 4;
					memcpy(&val, ptr, 4);
					commonVals[id] = val;
				}
				m_commonData[fieldId] = commonVals;
				offset += info.additional_data_size;
			}

			++fieldId;
		}

		memFile->seek(header.common_data_size, true);
	}

	// a section = 
	// 1. records
	// 2. string block
	// 3. id list
	// 4. copy table
	// 5. offset map 
	// 6. relationship map 
	// 7. offset map id list 

	uint32_t sectionSize = 0;
	if (header.section_count == 1)
		sectionSize = memFile->getSize() - m_sectionHeaders[0].file_offset;
	else
		sectionSize = m_sectionHeaders[1].file_offset - m_sectionHeaders[0].file_offset;

	const uint8_t* sectionData = memFile->getPointer();		//sectionSize
	const uint8_t* curPtr = sectionData;

	//1. record
	if ((header.flags & 0x01) == 0)
	{
		recordCount = m_sectionHeaders[0].record_count;
		m_recordOffsets.reserve(recordCount);
		for (uint32_t i = 0; i < recordCount; ++i)
			m_recordOffsets.push_back(sectionData + (i * recordSize));
	}
	else
	{
		curPtr += (m_sectionHeaders[0].offset_records_end - m_sectionHeaders[0].file_offset);
	}

	//2. string block
	curPtr += m_sectionHeaders[0].string_table_size;

	//3. id list
	if (m_sectionHeaders[0].id_list_size > 0)
	{
		uint32_t nId = m_sectionHeaders[0].id_list_size / 4;
		m_IDs.resize(nId);
		memcpy(m_IDs.data(), curPtr, nId * sizeof(uint32_t));
	} 
	else
	{
		m_IDs.reserve(recordCount);
		const auto& info = m_fieldStorageInfo[header.id_index];

		//read ids from data
		for (uint32_t i = 0; i < recordCount; ++i)
		{
			const uint8_t* recordOffset = sectionData + i * recordSize;
			switch (info.storage_type)
			{
			case FIELD_COMPRESSION::NONE:
				{
					uint8_t* val = new uint8_t[info.field_size_bits / 8];
					memcpy(val, recordOffset + info.field_offset_bits / 8, info.field_size_bits / 8);
					m_IDs.push_back((*reinterpret_cast<unsigned int*>(val)));
					delete[] val;
				}
				break;
				case FIELD_COMPRESSION::BITPACKED:
				{
					unsigned int size = (info.field_size_bits + (info.field_offset_bits & 7) + 7) / 8;
					unsigned int offset = info.field_offset_bits / 8;
					uint8_t* val = new unsigned char[size];
					memcpy(val, recordOffset + offset, size);
					unsigned int id = (*reinterpret_cast<unsigned int*>(val));
					delete[] val;
					id = id & ((1ull << info.field_size_bits) - 1);
					m_IDs.push_back(id);
				}
				break;
				case FIELD_COMPRESSION::COMMON_DATA:
					assert(false);
					return false;
				case FIELD_COMPRESSION::BITPACKED_INDEXED:
				case FIELD_COMPRESSION::BITPACKED_SIGNED:
				{
					uint32_t id = readBitpackedValue(info, recordOffset);
					m_IDs.push_back(id);
					break;
				}
				case FIELD_COMPRESSION::BITPACKED_INDEXED_ARRAY:
					assert(false);
					return false;
			default:
				assert(false);
				return false;
			}
		}
	}

	//4. copy table
	std::vector<copy_table_entry> copyTable;
	if (m_sectionHeaders[0].copy_table_count > 0)
	{
		uint32_t nbEntries = m_sectionHeaders[0].copy_table_count;
		memcpy(copyTable.data(), curPtr, nbEntries * sizeof(copy_table_entry));
		curPtr += (nbEntries * sizeof(copy_table_entry));
	}

	//5. offset map
	std::vector<offset_map_entry> offsetMap;
	if (m_sectionHeaders[0].offset_map_id_count > 0)
	{
		uint32_t nbEntries = m_sectionHeaders[0].offset_map_id_count;
		memcpy(offsetMap.data(), curPtr, nbEntries * sizeof(offset_map_entry));
		curPtr += (nbEntries * sizeof(offset_map_entry));
	}

	//6. relationship map
	if (m_sectionHeaders[0].relationship_data_size > 0)
	{
		uint32_t nbEntries;
		memcpy(&nbEntries, curPtr, 4);
		curPtr += (4 + 8);

		for (uint32_t i = 0; i < nbEntries; ++i)
		{
			uint32_t foreignKey;
			uint32_t recordIndex;
			memcpy(&foreignKey, curPtr, 4);
			curPtr += 4;
			memcpy(&recordIndex, curPtr, 4);
			curPtr += 4;
			m_relationShipData[recordIndex] = std_string_format("%u", foreignKey);
		}
	}

	//7. offset map id list
	if (m_sectionHeaders[0].offset_map_id_count > 0)
	{
		uint32_t nbEntries = m_sectionHeaders[0].offset_map_id_count;
		m_IDs.resize(nbEntries);
		memcpy(m_IDs.data(), curPtr, nbEntries * sizeof(uint32_t));
	}

	if (!offsetMap.empty())
	{
		m_recordOffsets.clear();
		m_isSparseTable = true;
		for (const auto& it : offsetMap)
			m_recordOffsets.push_back(sectionData - m_sectionHeaders[0].file_offset + it.offset);
	}

	recordCount = (uint32_t)m_recordOffsets.size();
	if (!copyTable.empty())
	{
		uint32_t nbEntries = (uint32_t)copyTable.size();

		m_IDs.reserve(recordCount + nbEntries);
		m_recordOffsets.reserve(recordCount + nbEntries);
		
		// create a id->offset map
		std::map<uint32_t, const uint8_t*> IDToOffsetMap;

		for (uint32_t i = 0; i < recordCount; ++i)
		{
			IDToOffsetMap[m_IDs[i]] = m_recordOffsets[i];
		}

		for (const copy_table_entry& entry : copyTable)
		{
			m_IDs.push_back(entry.newRowId);
			m_recordOffsets.push_back(IDToOffsetMap[entry.copiedRowId]);
		}
		recordCount += nbEntries;
	}

	return true;
}

std::vector<VAR_T> WDC3File::getRecordValue(uint32_t index, const CTableStruct * table) const
{
	std::vector<VAR_T> result;

	const uint8_t* recordOffset = m_recordOffsets[index];

	std::vector<uint8_t> vals;
	for (const auto& field : table->fields)
	{
		VAR_T v;

		if (field.isKey)
		{
			v = (uint32_t)m_IDs[index];
			result.push_back(v);
			continue;
		}
	}

	return result;
}
