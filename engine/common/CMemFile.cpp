#include "CMemFile.h"

CMemFile::CMemFile(const uint8_t* buf, uint32_t size)
	: buffer(buf), size(size), pointer(0), eof(false)
{
}

CMemFile::~CMemFile()
{
	close();
}

uint32_t CMemFile::read( void* dest, uint32_t bytes )
{
	if (eof) 
		return 0;

	size_t rpos = pointer + bytes;
	if (rpos >= size) {
		bytes = size - pointer;
		eof = true;
	}

	memcpy(dest, &(buffer[pointer]), bytes);

	pointer = (uint32_t)rpos;

	return bytes;
}

bool CMemFile::seek( int32_t offset, bool relative )
{
	if (relative)
		pointer += offset;
	else
		pointer = offset;
	eof = (pointer >= size);
	return !eof;
}

void CMemFile::close()
{
	delete[] buffer;
	buffer = nullptr;

	eof = true;
}
