#pragma once

#include <string>

class CMemFile
{
public:
	CMemFile(uint8_t* buf, uint32_t size, const char* fname);
	~CMemFile();

	uint32_t		read(void* dest, uint32_t bytes);
	uint32_t		getSize() const { return size; }
	uint32_t		getPos() const { return pointer; }
	uint8_t*		getBuffer() { return buffer; }
	uint8_t*		getPointer() { return buffer + pointer; }
	bool		isEof() const { return eof; }
	bool		seek(int32_t offset, bool relative = false);
	void		close();
	const char*	getFileName() const { return filename.c_str(); }

private:
	uint8_t*		buffer;
	uint32_t		pointer;
	uint32_t		size;
	std::string		filename;
	bool eof;
};