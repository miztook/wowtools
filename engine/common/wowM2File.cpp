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

	ASSERT(memFile->getSize() >= sizeof(M2::Header));

	GameFile gameFile(memFile);
	if (gameFile.isChunked())
	{
		if (gameFile.setChunk("MD21"))
		{
			memFile->seek(gameFile.getFileOffset());
		}
		else
		{
			delete memFile;
			return false;
		}
	}

	memcpy(&Header, gameFile.getFileData(), sizeof(Header));

	char meshName[DEFAULT_SIZE];
	Q_strncpy(meshName, DEFAULT_SIZE, (const char*)&gameFile.getFileData()[Header._modelNameOffset], Header._modelNameLength);
	meshName[Header._modelNameLength] = '\0';

	const char* sharp = strstr(meshName, "#");
	if (sharp)
		Name = std::string(meshName, sharp - meshName);
	else
		Name = meshName;

	FileName = getFileNameNoExtensionA(filename);
	Dir = getFileDirA(filename);
	normalizeDirName(Dir);

	Type = getM2Type(Dir.c_str());				//½ÇÉ«npc

	if (gameFile.isChunked() && gameFile.setChunk("SKID"))
	{
		memFile->seek(gameFile.getFileOffset());


		gameFile.setChunk("MD21");
		memFile->seek(gameFile.getFileOffset());
	}
	else
	{

	}

	delete memFile;
	return true;
}

