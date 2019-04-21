#include "wowWDB5File.h"

#include "CMemFile.h"
#include "wowDatabase.h"
#include <cassert>

WDB5File::WDB5File(CMemFile* memFile)
	: DBFile(memFile), m_isSparseTable(false)
{
}

bool WDB5File::open()
{
	if (!m_pMemFile)
	{
		assert(false);
		return false;
	}

	const uint8_t* buffer = m_pMemFile->getBuffer();

	WDB5File::header header;
	m_pMemFile->read(&header, sizeof(header));

	recordSize = header.record_size;
	recordCount = header.record_count;
	fieldCount = header.field_count;
	stringSize = header.string_table_size;

	//field
	std::vector<WDB5File::field_structure> fields;
	fields.resize(fieldCount);
	m_pMemFile->read(fields.data(), fieldCount * sizeof(WDB5File::field_structure));
	for (const auto& field : fields)
	{
		m_fieldSizes[field.position] = field.size;
	}

	//
	data = m_pMemFile->getPointer();
	stringTable = data + recordSize * recordCount;

	if ((header.flags & 0x01) != 0)
	{
		m_isSparseTable = true;
		m_pMemFile->seek(stringSize);

		recordCount = 0;

		for (uint32_t i = 0; i < (header.max_id - header.min_id + 1); ++i)
		{
			uint32_t offset;
			uint16_t length;

			m_pMemFile->read(&offset, sizeof(offset));
			m_pMemFile->read(&length, sizeof(length));

			if ((offset == 0) || (length == 0))
				continue;

			m_IDs.push_back(header.min_id + i);
			m_recordOffsets.push_back(buffer + offset);
			++recordCount;
		}
	}
	else
	{
		m_IDs.reserve(recordCount);
		m_recordOffsets.reserve(recordCount);

		// read IDs
		m_pMemFile->seek(recordSize*recordCount + stringSize, true);
		if ((header.flags & 0x04) != 0)
		{
			m_IDs.resize(recordCount);
			m_pMemFile->read(m_IDs.data(), recordCount * sizeof(uint32_t));
		}
		else
		{
			uint32_t indexPos = fields[header.id_index].position;
			uint32_t indexSize = ((32 - fields[header.id_index].size) / 8);
			uint32_t indexMask = 0xFFFFFFFF;
			if (indexSize == 1)
				indexMask = 0x000000FF;
			else if (indexSize == 2)
				indexMask = 0x0000FFFF;
			else if (indexSize == 3)
				indexMask = 0x00FFFFFF;

			// read ids from data
			for (uint32_t i = 0; i < recordCount; i++)
			{
				const uint8_t* recordOffset = data + (i*recordSize);
				uint32_t v;
				memcpy(&v, recordOffset + indexPos, indexSize);
				m_IDs.push_back(v & indexMask);
			}
		}

		// store offsets
		for (uint32_t i = 0; i < recordCount; i++)
			m_recordOffsets.push_back(data + (i*recordSize));
	}

	// copy table
	if (header.copy_table_size > 0)
	{
		uint32_t nbEntries = header.copy_table_size / sizeof(copy_table_entry);

		m_IDs.reserve(recordCount + nbEntries);
		m_recordOffsets.reserve(recordCount + nbEntries);

		std::vector<copy_table_entry> copyTable;
		copyTable.resize(nbEntries); 
		m_pMemFile->read(copyTable.data(), header.copy_table_size);

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

std::vector<VAR_T> WDB5File::getRecordValue(uint32_t index, const CTableStruct* table) const
{
	std::vector<VAR_T> result;

	const uint8_t* recordOffset = m_recordOffsets[index];

	std::vector<uint8_t> vals;
	for (const auto& field : table->fields )
	{
		if (field.isCommonData)
			continue;

		for (uint32_t i = 0; i < field.arraySize; ++i)
		{
			int fieldSize = (32 - m_fieldSizes.at(field.pos)) / 8;
			vals.resize(fieldSize);
			memcpy(vals.data(), recordOffset + field.pos + i * fieldSize, fieldSize);
		
			VAR_T v;
			if (field.type == "text")
			{
				const char* stringPtr;
				if (m_isSparseTable)
					stringPtr = reinterpret_cast<const char *>(recordOffset + field.pos);
				else
					stringPtr = reinterpret_cast<const char *>(stringTable + *reinterpret_cast<int *>(vals.data()));

				v = std::string(stringPtr);
			}
			else if (field.type == "float")
			{
				v = *reinterpret_cast<float*>(vals.data());
			}
			else if (field.type == "int")
			{
				v = *reinterpret_cast<int*>(vals.data());
			}
			else
			{
				unsigned int mask = 0xFFFFFFFF;
				if (fieldSize == 1)
					mask = 0x000000FF;
				else if (fieldSize == 2)
					mask = 0x0000FFFF;
				else if (fieldSize == 3)
					mask = 0x00FFFFFF;
				
				v = (*reinterpret_cast<uint32_t*>(vals.data()) & mask);	
			}
			result.push_back(v);
		}
	}
	return result;
}
