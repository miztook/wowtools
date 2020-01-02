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

		ASSERT_TODO

		gameFile.setChunk("MD21");
		memFile->seek(gameFile.getFileOffset());
	}

	if (gameFile.isChunked() && gameFile.setChunk("SFID"))
	{
		memFile->seek(gameFile.getFileOffset());

		if (Header._nViews > 0)
		{
			for (uint32_t i = 0; i < Header._nViews; ++i)
			{
				uint32_t skinFile;
				memFile->read(&skinFile, sizeof(uint32_t));
				SkinFileIDs.push_back(skinFile);
			}
		}

		gameFile.setChunk("MD21");
		memFile->seek(gameFile.getFileOffset());
	}

	const uint8_t* filebuffer = memFile->getPointer();

	//
	loadVertices(filebuffer);

	loadBounds(filebuffer);

	loadTextures(filebuffer);


	delete memFile;
	return true;
}

void wowM2File::loadVertices(const uint8_t* fileStart)
{
	if (Header._nVertices == 0)
		return;

	Vertices.resize(Header._nVertices);

	M2::vertex* v = (M2::vertex*)(&fileStart[Header._ofsVertices]);
	for (uint32_t i = 0; i < Header._nVertices; ++i)
	{
		Vertices[i].Pos = M2::fixCoordinate(v[i]._Position);
		Vertices[i].Normal = M2::fixCoordinate(v[i]._Normal);
		Vertices[i].TCoords0 = v[i]._TextureCoords0;
		Vertices[i].TCoords1 = v[i]._TextureCoords1;
		for (uint32_t j = 0; j < 4; ++j)
		{
			Vertices[i].Weights[j] = v[i]._BoneWeight[j];
			Vertices[i].BoneIndices[j] = v[i]._BoneIndices[j];
		}

		BoundingBox.addInternalPoint(Vertices[i].Pos);
	}
}

void wowM2File::loadBounds(const uint8_t* fileStart)
{
	if (Header._nBoundingVertices > 0)
	{
	}

	if (Header._nBoundingTriangles > 0)
	{
	}

	BoundingAABBox = aabbox3df(M2::fixCoordinate(Header._boundingbox.MinEdge), M2::fixCoordinate(Header._boundingbox.MaxEdge));
	BoundingRadius = Header._boundingRadius;
	CollisionAABBox = aabbox3df(M2::fixCoordinate(Header._vertexBox.MinEdge), M2::fixCoordinate(Header._vertexBox.MaxEdge));
	CollisionRadius = Header._vertexRadius;
}

void wowM2File::loadTextures(const uint8_t* fileStart)
{

}

