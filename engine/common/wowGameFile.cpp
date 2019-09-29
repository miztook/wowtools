#include "wowGameFile.h"

const char* GameFile::WowChunks[] =
{
	"PFID",
	"SFID",
	"AFID",
	"BFID",
	"MD21",
	"TXAC",
	"EXPT",
	"EXP2",
	"PABC",
	"PADC",
	"PSBC",
	"PEDC",
	"SKID",
	"TXID",
	"LDV1",
	"AFM2",
	"AFSA",
	"AFSB",
	"SKL1",
	"SKA1",
	"SKB1",
	"SKS1",
	"SKPD",
};

GameFile::GameFile(CMemFile * memFile)
	: m_pMemFile(memFile)
{
	FileData = nullptr;
	FileSize = 0;
	FileOffset = 0;

	afterOpen();
}

GameFile::~GameFile()
{
	delete m_pMemFile;
}

bool GameFile::setChunk(const char* chunkName)
{
	for (const auto& chunk : Chunks)
	{
		if (chunk.magic == chunkName)
		{
			FileData = m_pMemFile->getBuffer() + chunk.start;
			FileSize = chunk.size;
			FileOffset = chunk.start;

			return true;
		}
	}

	return false;
}

void GameFile::afterOpen()
{
	uint32_t filesize = m_pMemFile->getSize();
	const uint8_t* buffer = m_pMemFile->getBuffer();

	if (filesize > sizeof(ChunkHeader))
	{
		ChunkHeader header;
		memcpy(&header, buffer, sizeof(ChunkHeader));

		bool bfind = false;
		for (const char* chunk : WowChunks)
		{
			if (strncmp(chunk, header.magic, 4) == 0)
			{
				bfind = true;
				break;
			}
		}

		if (bfind && (header.size + sizeof(ChunkHeader)) <= filesize)
		{
			uint32_t offset = 0;
			while (offset < filesize)
			{
				ChunkHeader chunkHeader;
				memcpy(&chunkHeader, buffer, sizeof(ChunkHeader));
				offset += sizeof(ChunkHeader);

				Chunk chunk;
				chunk.magic = std::string(chunkHeader.magic, 4);
				chunk.start = offset;
				chunk.size = chunkHeader.size;
				Chunks.push_back(chunk);

				offset += chunkHeader.size;
			}
		}
	}
}