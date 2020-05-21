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
	// 3. id list
	// 4. copy table
	// 5. relationship map  

	m_pMemFile->seek(m_sectionHeaders[0].file_offset, false);


}
