#pragma once

#include <string>

class CMemFile
{
public:
	CMemFile(const uint8_t* buf, uint32_t size, const char* fname);
	~CMemFile();

	uint32_t		read(void* dest, uint32_t bytes);
	uint32_t		getSize() const { return size; }
	uint32_t		getPos() const { return pointer; }
	const uint8_t*		getBuffer() const { return buffer; }
	const uint8_t*		getPointer() const { return buffer + pointer; }
	bool		isEof() const { return eof; }
	bool		seek(int32_t offset, bool relative = false);
	void		close();
	const char*	getFileName() const { return filename.c_str(); }

private:
	const uint8_t*		buffer;
	uint32_t		pointer;
	uint32_t		size;
	std::string		filename;
	bool eof;
};