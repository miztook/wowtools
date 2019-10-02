#include "wowM2File.h"
#include "CMemFile.h"
#include "stringext.h"

#include "wowEnvironment.h"
#include "wowGameFile.h"

wowM2File::wowM2File(const wowEnvironment* wowEnv)
	: WowEnvironment(wowEnv)
{
}

wowM2File::~wowM2File()
{
}

bool wowM2File::loadFile(const char* filename)
{
	CMemFile* memFile = WowEnvironment->openFile(filename);
	if (!memFile)
		return false;

	ASSERT(memFile->getSize() >= sizeof(M2::Header20));

	GameFile gameFile(memFile);
	if (gameFile.isChunked() && !gameFile.setChunk("MD21"))
	{
		delete memFile;
		return false;
	}

	memcpy(&Header, gameFile.getFileData(), sizeof(Header));

	FileName = getFileNameNoExtensionA(filename);
	Dir = getFileDirA(filename);
	normalizeDirName(Dir);

	Type = getM2Type(Dir.c_str());				//½ÇÉ«npc



	delete memFile;
	return true;
}

