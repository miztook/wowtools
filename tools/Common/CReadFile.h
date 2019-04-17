#pragma once

#include <cstdint>
#include <cstdio>
#include <string>

#define	MAX_READ_NUM		1024

class CReadFile
{
public:
	CReadFile(const char* filename, bool binary);
	 ~CReadFile();

public:
	 uint32_t read(void* buffer, uint32_t sizeToRead);
	 uint32_t readText(char* buffer, uint32_t len = MAX_READ_NUM);
	 uint32_t readLine(char* buffer, uint32_t len = MAX_READ_NUM);
	 uint32_t readLineSkipSpace(char* buffer, uint32_t len = MAX_READ_NUM);
	 bool seek(int32_t finalPos, bool relativePos = false);
	 uint32_t getSize() const { return FileSize; }
	 int32_t getPos() const;
	 bool isEof() const;
	 const char* getFileName() const { return FileName.c_str(); }
	 bool isOpen() const { return File != nullptr; }
	 bool isBinary() const { return IsBinary; }

private:
	void openFile(bool binary);
	bool isWhiteSpace(const char symbol) const
	{
		return symbol == ' ' || symbol == '\t' || symbol == '\r';
	}

private:
	FILE*		File;
	uint32_t		FileSize;
	std::string		FileName;
	bool		IsBinary;
};