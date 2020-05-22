#include "wowWDC3File.h"

#include "CMemFile.h"
#include "wowDatabase.h"
#include "stringext.h"
#include <cassert>

WDC3File::WDC3File(CMemFile* memFile) 
	: DBFile(memFile), m_isSparseTable(false)
{
	memset(&m_header, 0, sizeof(m_header));
}

bool WDC3File::open()
{
	if (!m_pMemFile)
	{
		ASSERT(false);
		return false;
	}

	const uint8_t* buffer = m_pMemFile->getBuffer();

	m_pMemFile->read(&m_header, sizeof(m_header));

	recordSize = m_header.record_size;
	recordCount = m_header.record_count;
	fieldCount = m_header.field_count;
	stringSize = m_header.string_table_size;

	//section header
	m_sectionHeaders.resize(m_header.section_count);
	m_pMemFile->read(m_sectionHeaders.data(), sizeof(section_header) * m_header.section_count);

	m_isSparseTable = m_sectionHeaders[0].offset_map_id_count > 0;

	//field
	std::vector<WDC3File::field_structure> fields;
	fields.resize(fieldCount);
	m_pMemFile->read(fields.data(), fieldCount * sizeof(WDC3File::field_structure));
	for (const auto& field : fields)
	{
		m_fieldSizes[field.position] = field.size;
	}

	//storage info
	if (m_header.field_storage_info_size > 0)
	{
		uint32_t nFieldStorageInfo = m_header.field_storage_info_size / sizeof(field_storage_info);
		m_fieldStorageInfo.resize(nFieldStorageInfo);
		m_pMemFile->read(m_fieldStorageInfo.data(), sizeof(field_storage_info) * nFieldStorageInfo);
	}

	//pallte data
	if (m_header.pallet_data_size > 0)
	{
		m_palletData.resize(m_header.pallet_data_size);
		m_pMemFile->read(m_palletData.data(), m_header.pallet_data_size);

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
	if (m_header.common_data_size > 0)
	{
		const uint8_t* commonData = m_pMemFile->getPointer();

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
					ptr += 4;

					commonVals[id] = val;
				}
				m_commonData[fieldId] = commonVals;
				offset += info.additional_data_size;
			}

			++fieldId;
		}

		m_pMemFile->seek(m_header.common_data_size, true);
	}

	// a section = 
	// 1. records
	// 2. string block
	// 3. id list
	// 4. copy table
	// 5. offset map 
	// 6. relationship map 
	// 7. offset map id list 

	ASSERT(m_pMemFile->getPos() == m_sectionHeaders[0].file_offset);

	uint32_t sectionSize = 0;
	if (m_header.section_count == 1)
		sectionSize = m_pMemFile->getSize() - m_sectionHeaders[0].file_offset;
	else
		sectionSize = m_sectionHeaders[1].file_offset - m_sectionHeaders[0].file_offset;

	const uint8_t* sectionData = m_pMemFile->getPointer();		//sectionSize
	m_pMemFile->seek(sectionSize, true);

	const uint8_t* curPtr = sectionData;

	//1. record
	if ((m_header.flags & 0x01) == 0)  // non sparse table
	{
		recordCount = m_sectionHeaders[0].record_count;
		m_recordOffsets.reserve(recordCount);
		for (uint32_t i = 0; i < recordCount; ++i)
			m_recordOffsets.push_back(sectionData + (i * recordSize));

		curPtr += (recordSize * recordCount);
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

		curPtr += m_sectionHeaders[0].id_list_size;
	} 
	else
	{
		m_IDs.reserve(recordCount);
		const auto& info = m_fieldStorageInfo[m_header.id_index];

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
					ASSERT(false);
					return false;
				case FIELD_COMPRESSION::BITPACKED_INDEXED:
				case FIELD_COMPRESSION::BITPACKED_SIGNED:
				{
					uint32_t id = readBitpackedValue(info, recordOffset);
					m_IDs.push_back(id);
					break;
				}
				case FIELD_COMPRESSION::BITPACKED_INDEXED_ARRAY:
					ASSERT(false);
					return false;
			default:
				ASSERT(false);
				return false;
			}
		}
	}

	//4. copy table
	std::vector<copy_table_entry> copyTable;
	if (m_sectionHeaders[0].copy_table_count > 0)
	{
		uint32_t nbEntries = m_sectionHeaders[0].copy_table_count;
		copyTable.resize(nbEntries);
		memcpy(copyTable.data(), curPtr, nbEntries * sizeof(copy_table_entry));
		curPtr += (nbEntries * sizeof(copy_table_entry));
	}

	//5. offset map
	std::vector<offset_map_entry> offsetMap;
	if (m_sectionHeaders[0].offset_map_id_count > 0)
	{
		uint32_t nbEntries = m_sectionHeaders[0].offset_map_id_count;
		offsetMap.resize(nbEntries);
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
			m_relationShipData[recordIndex] = foreignKey;
		}
	}

	//7. offset map id list
	if (m_sectionHeaders[0].offset_map_id_count > 0)
	{
		uint32_t nbEntries = m_sectionHeaders[0].offset_map_id_count;
		m_IDs.resize(nbEntries);
		memcpy(m_IDs.data(), curPtr, nbEntries * sizeof(uint32_t));

		curPtr += nbEntries * sizeof(uint32_t);
	}

	if (!offsetMap.empty())
	{
		m_recordOffsets.clear();
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

	ASSERT(curPtr - sectionData == sectionSize);			//read section finished

	return true;
}

std::vector<VAR_T> WDC3File::getRecordValue(uint32_t index, const CTableStruct* table) const
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

		if (field.isRelationshipData)
		{
			uint32_t key;
			auto itr = m_relationShipData.find(index);
			if (itr != m_relationShipData.end())
				key = itr->second;
			else
				key = 0;
			v = key;
			result.push_back(v);
			continue;
		}

		for (uint32_t i = 0; i < field.arraySize; ++i)
		{
			uint32_t val = 0;
			if (!readFieldValue(index, field.pos, i, field.arraySize, val))
				continue;

			if (field.type == "text")
			{
				const char* strPtr;
				if (m_isSparseTable)
				{
					const uint8_t* ptr = recordOffset;
					for (int f = 0; f <= field.pos; ++f)
					{
						if (table->fields[f].isKey)
							continue;
						if (table->fields[f].type == "uint64")
						{
							ptr += 8;
						}
						else
						{
							ptr += (strlen((const char*)ptr) + 1);
						}
					}
					strPtr = reinterpret_cast<const char*>(ptr);
				}
				else
				{
					strPtr = reinterpret_cast<const char*>(recordOffset + m_fieldStorageInfo[field.pos].field_offset_bits / 8 + val
						- ((m_header.record_count - m_sectionHeaders[0].record_count) * m_header.record_size));
				}
				v = std::string(strPtr);
				result.push_back(v);
			}
			else if (field.type == "float")
			{
				v = *reinterpret_cast<float*>(&val);
				result.push_back(v);
			}
			else if (field.type == "int")
			{
				v = *reinterpret_cast<int*>(&val);
				result.push_back(v);
			}
			else if (field.type == "uint16")
			{
				v = *reinterpret_cast<uint16_t*>(&val);
				result.push_back(v);
			}
			else if (field.type == "byte")
			{
				v = (uint16_t)(*reinterpret_cast<uint16_t*>(&val) & 0x000000FF);
				result.push_back(v);
			}
			else if (field.type == "uint64")
			{
				v = *reinterpret_cast<uint64_t*>(&val);
				result.push_back(v);
			}
			else
			{
				v = *reinterpret_cast<uint32_t*>(&val);
				result.push_back(v);
			}
		}

	}

	return result;
}

bool WDC3File::readFieldValue(uint32_t recordIndex, uint32_t fieldIndex, uint32_t arrayIndex, uint32_t arraySize, uint32_t & result) const
{
	const uint8_t* recordOffset = m_recordOffsets[recordIndex];
	const auto& info = m_fieldStorageInfo[fieldIndex];

	switch (info.storage_type)
	{
	case FIELD_COMPRESSION::NONE:
	{
		uint16_t fieldSize = info.field_size_bits / 8;
		const uint8_t* fieldOffset = recordOffset + info.field_offset_bits / 8;

		if (arraySize != 1)
		{
			fieldSize /= arraySize;
			fieldOffset += ((info.field_size_bits / 8 / arraySize) * arrayIndex);
		}

		uint8_t* val = new uint8_t[fieldSize];
		memcpy(val, fieldOffset, fieldSize);
		// handle special case => when value is supposed to be 0, values read are all 0xFF
		// Don't understand why, so I use this ugly stuff...
		if (arraySize != 1)
		{
			uint8_t nbFF = 0;
			for (uint8_t i = 0; i < fieldSize; i++)
			{
				if (val[i] == 0xFF)
					nbFF++;
			}

			if (nbFF == fieldSize)
			{
				for (uint8_t i = 0; i < fieldSize; i++)
					val[i] = 0;
			}
		}
		result = (*reinterpret_cast<uint32_t*>(val));
		result = result & ((1ull << (info.field_size_bits / arraySize)) - 1);
		delete[] val;
	}
	break;
	case FIELD_COMPRESSION::BITPACKED:
	case FIELD_COMPRESSION::BITPACKED_SIGNED:
	{
		result = readBitpackedValue(info, recordOffset);
	}
	break;
	case FIELD_COMPRESSION::COMMON_DATA:
	{
		result = info.val1;
		auto mapIt = m_commonData.find(fieldIndex);
		if (mapIt != m_commonData.end())
		{
			auto valIt = mapIt->second.find(m_IDs[recordIndex]);
			if (valIt != mapIt->second.end())
				result = valIt->second;
		}
	}
	break;
	case FIELD_COMPRESSION::BITPACKED_INDEXED:
	{
		uint32_t index = readBitpackedValue(info, recordOffset);
		auto it = m_palletBlockOffsets.find(fieldIndex);
		uint32_t offset = it->second + index * 4;
		memcpy(&result, &m_palletData[offset], 4);
	}
	break;
	case FIELD_COMPRESSION::BITPACKED_INDEXED_ARRAY:
	{
		uint32_t index = readBitpackedValue(info, recordOffset);
		auto it = m_palletBlockOffsets.find(fieldIndex);
		uint32_t offset = it->second + index * arraySize * 4 + arrayIndex * 4;
		memcpy(&result, &m_palletData[offset], 4);
	}
	break;
	default:
		ASSERT(false);
		return false;
	}
	return true;
}

uint32_t WDC3File::readBitpackedValue(const field_storage_info& info, const uint8_t* recordOffset) const
{
	uint16_t size = (info.field_size_bits + (info.field_offset_bits & 7) + 7) / 8;
	uint16_t offset = info.field_offset_bits / 8;
	
	uint8_t* vals = new uint8_t[size];
	memcpy(vals, recordOffset + offset, size);

	uint32_t result = (*reinterpret_cast<uint32_t*>(vals));
	delete[] vals;

	result = result >> (info.field_offset_bits & 7);
	result = result & ((1ull << info.field_size_bits) - 1);
	return result;
}

