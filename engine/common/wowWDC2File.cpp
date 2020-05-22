#include "wowWDC2File.h"
#include "CMemFile.h"
#include "wowDatabase.h"
#include "stringext.h"
#include <cassert>

WDC2File::WDC2File(CMemFile * memFile)
	: DBFile(memFile), m_isSparseTable(false)
{
	memset(&m_header, 0, sizeof(m_header));
}

bool WDC2File::open()
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

	m_isSparseTable = (m_header.flags & 0x01) != 0;

	//field
	std::vector<WDC2File::field_structure> fields;
	fields.resize(fieldCount);
	m_pMemFile->read(fields.data(), fieldCount * sizeof(WDC2File::field_structure));
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
		m_pMemFile->seek(m_header.pallet_data_size, true);

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
	// 3. id offset
	// 4. copy table
	// 5. relationship map  

	m_pMemFile->seek(m_sectionHeaders[0].file_offset, false);
	const uint8_t* sectionData = m_pMemFile->getPointer();		//sectionSize

	const uint32_t stringTableOffset = m_pMemFile->getPos() + recordSize * recordCount;
	const uint32_t idBlockOffset = stringTableOffset + stringSize;
	const uint32_t copyBlockOffset = idBlockOffset + m_sectionHeaders[0].id_list_size;
	const uint32_t relationshipDataOffset = copyBlockOffset + m_sectionHeaders[0].copy_table_size;

	// embedded strings in fields instead of stringTable
	if (m_isSparseTable)
	{
		stringSize = 0;
	}

	//2. string block
	m_pMemFile->seek(stringTableOffset, false);

	//3. id offset
	if (m_isSparseTable)
	{
		m_pMemFile->seek(m_sectionHeaders[0].file_offset, false);

		recordCount = 0;
		for (uint32_t i = 0; i < (m_header.max_id - m_header.min_id + 1); ++i)
		{
			uint32_t offset;
			uint16_t length;

			m_pMemFile->read(&offset, sizeof(offset));
			m_pMemFile->read(&length, sizeof(length));

			if (offset == 0 || length == 0)
				continue;

			m_IDs.push_back(m_header.min_id + i);
			m_recordOffsets.push_back(buffer + offset);

			++recordCount;
		}
	}
	else
	{
		m_recordOffsets.reserve(recordCount);

		//read IDs
		if ((m_header.flags & 0x04) != 0)
		{
			m_pMemFile->seek(idBlockOffset, false);

			m_IDs.resize(recordCount);
			m_pMemFile->read(m_IDs.data(), recordCount * sizeof(uint32_t));
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

		// store offsets
		for (uint32_t i = 0; i < recordCount; i++)
			m_recordOffsets.push_back(sectionData + (i*recordSize));
	}
}
