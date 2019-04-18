#pragma once

#include <stdint.h>

class DBFile
{
public:
	DBFile();
	virtual ~DBFile();

public:
	virtual bool open() = 0;
	virtual void close() = 0;

protected:
	uint32_t recordSize;
	uint32_t recordCount;
	uint32_t fieldCount;
	uint32_t stringSize;
	unsigned char *data;
	unsigned char *stringTable;
};