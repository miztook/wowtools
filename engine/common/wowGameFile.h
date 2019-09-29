#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include "predefine.h"
#include "CMemFile.h"

class GameFile
{
public:
	explicit GameFile(CMemFile* memFile);

	virtual ~GameFile();

public:
	bool setChunk(const char* chunkName);
	
	const uint8_t* getFileData() const { return FileData; }
	uint32_t getFileSize() const { return FileSize; }
	uint32_t getFileOffset() const { return FileOffset; }
	bool isChunked() const { return !Chunks.empty(); }

protected:
	struct ChunkHeader
	{
		char magic[4];
		uint32_t size;
	};

	struct Chunk
	{
		std::string magic;
		uint32_t size;
		uint32_t start;
	};

	void afterOpen();

protected:
	CMemFile*	m_pMemFile;
	const uint8_t*	FileData;
	uint32_t	FileSize;
	uint32_t	FileOffset;
	std::vector<Chunk>	Chunks;

	static const char* WowChunks[];
};