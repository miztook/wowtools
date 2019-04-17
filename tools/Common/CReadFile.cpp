#include "CReadFile.h"
#include "stringext.h"
#include "function.h"

CReadFile::CReadFile(const char* filename, bool binary)
	: IsBinary(binary)
{
	FileName = filename;
	normalizeFileName(FileName);
	openFile(IsBinary);
}

CReadFile::~CReadFile()
{
	if (File)
		fclose(File);
}

uint32_t CReadFile::read(void* buffer, uint32_t sizeToRead)
{
	if (!isOpen() || buffer == nullptr)
		return 0;

	return (uint32_t)fread(buffer, 1, sizeToRead, File);
}

uint32_t CReadFile::readText(char* buffer, uint32_t len /*= MAX_READ_NUM */)
{
	if (!isOpen() || buffer == nullptr)
		return 0;

	assert(!IsBinary);

	char* c = fgets(buffer, len, File);
	return c ? (uint32_t)strlen(c) : 0;
}

uint32_t CReadFile::readLine(char* buffer, uint32_t len /*= MAX_READ_NUM*/)
{
	if (!isOpen() || buffer == nullptr)
		return 0;

	assert(!IsBinary);

	int c = fgetc(File);
	uint32_t count = 0;
	while (c != '\n' && c != '\r' && c != EOF)
	{
		if (1 + count >= len)
			break;
		buffer[count] = c;
		++count;

		c = fgetc(File);
	}
	buffer[count] = '\0';

	return count;
}

uint32_t CReadFile::readLineSkipSpace(char* buffer, uint32_t len /*= MAX_READ_NUM*/)
{
	if (!isOpen() || buffer == nullptr)
		return 0;

	memset(buffer, 0, len);

	assert(!IsBinary);

	int c = fgetc(File);
	uint32_t count = 0;

	while (c != '\n' && c != '\r' && c != EOF)
	{
		if (1 + count >= len)
			break;

		if (!isWhiteSpace((char)c))
		{
			buffer[count] = c;
			++count;
		}
		c = fgetc(File);
	}
	buffer[count] = '\0';

	return count;
}

bool CReadFile::seek(int32_t finalPos, bool relativePos/* = false*/)
{
	if (!isOpen())
		return false;

	return fseek(File, finalPos, relativePos ? SEEK_CUR : SEEK_SET) == 0;
}

int32_t CReadFile::getPos() const
{
	return (int32_t)ftell(File);
}

bool CReadFile::isEof() const
{
	return getPos() == (int32_t)FileSize;
}

void CReadFile::openFile(bool binary)
{
	File = Q_fopen(FileName.c_str(), binary ? "rb" : "rt");
	if (File)
	{
		fseek(File, 0, SEEK_END);
		long size = ftell(File);
		FileSize = size > 0 ? (uint32_t)size : 0;
		fseek(File, 0, SEEK_SET);
	}
	else
	{
		FileSize = 0;
	}
}
