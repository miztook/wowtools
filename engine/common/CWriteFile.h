#pragma once

#include <cstdint>
#include <cstdio>
#include <string>

#define MAX_WRITE_NUM	1024

class CWriteFile
{
public:
	CWriteFile(const char* filename, bool binary, bool append = false);
	~CWriteFile();

public:
	uint32_t write(const void* buffer, uint32_t sizeToWrite);
	uint32_t writeText(const char* buffer, uint32_t len = MAX_WRITE_NUM);
	uint32_t writeLine(const char* text);
	bool flush();
	bool seek(int32_t finalPos, bool relativeMovement = false);
	uint32_t getSize() const { return FileSize; }
	uint32_t getPos() const;
	const char* getFileName() const { return FileName.c_str(); }
	bool isOpen() const { return File != nullptr; }
	bool isBinary() const { return IsBinary; }

private:
	void openFile(bool binary, bool append);

private:
	std::string FileName;
	FILE*	File;
	uint32_t	FileSize;
	bool IsBinary;
};