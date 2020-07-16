#include "wowM2File.h"
#include "CMemFile.h"
#include "stringext.h"

#include "wowEnvironment.h"
#include "wowGameFile.h"

wowM2File::wowM2File(const wowEnvironment* wowEnv)
	: WowEnvironment(wowEnv), SkinFile(this)
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

	loadSequences(filebuffer);

	loadColor(filebuffer);

	loadTransparency(filebuffer);

	loadTextureAnimation(filebuffer);

	loadBones(filebuffer);

	loadRenderFlags(filebuffer);

	loadAttachments(filebuffer);

	loadParticleSystems(filebuffer);

	loadRibbonEmitters(filebuffer);

	delete memFile;

	if (!loadSkin(0))
	{
		return false;
	}

	return true;
}

void wowM2File::loadVertices(const uint8_t* fileStart)
{
	if (Header._nVertices == 0)
		return;

	Vertices.resize(Header._nVertices);

	const M2::vertex* v = (M2::vertex*)(&fileStart[Header._ofsVertices]);
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
	if (Header._nTextures > 0)
	{
		const M2::texture* texDef = (M2::texture*)(&fileStart[Header._ofsTextures]);
		TexDefs.resize(Header._nTextures);
		for (uint32_t i = 0; i < Header._nTextures; ++i)
		{
			TexDefs[i].flags = texDef[i]._flags;
			TexDefs[i].type = (ETextureTypes)texDef[i]._type;

			if (TexDefs[i].type == 0)
			{
				const char* name = (const char*)(&fileStart[texDef[i]._ofsFilename]);
				uint32_t len = texDef[i]._lenFilename;
				TexDefs[i].filename = std::string(name, len);
			}
		}
	}
	
	if (Header._nTexLookup > 0)
	{
		const int16_t* t = (int16_t*)(&fileStart[Header._ofsTexLookup]);
		TexLookups.resize(Header._nTexLookup);
		memcpy(TexLookups.data(), t, sizeof(int16_t) * Header._nTexLookup);
	}
}

void wowM2File::loadSequences(const uint8_t* fileStart)
{
	if (Header._nGlobalSequences > 0)
	{
		int32_t* p = (int32_t*)(&fileStart[Header._ofsGlobalSequences]);
		GlobalSequences.resize(Header._nGlobalSequences);
		memcpy(GlobalSequences.data(), p, sizeof(int32_t) * Header._nGlobalSequences);
	}
}

void wowM2File::loadColor(const uint8_t* fileStart)
{
	if (Header._nColors > 0)
	{
		const M2::colorDef* c = (M2::colorDef*)(&fileStart[Header._ofsColors]);
		Colors.resize(Header._nColors);
		for (uint32_t i = 0; i < Header._nColors; ++i)
		{
			Colors[i].init(fileStart, GlobalSequences.data(), (uint32_t)GlobalSequences.size(), c[i]);
		}
	}
}

void wowM2File::loadTransparency(const uint8_t* fileStart)
{
	if (Header._nTransparency > 0)
	{
		const M2::transDef* t = (M2::transDef*)(&fileStart[Header._ofsTransparency]);
		Transparencies.resize(Header._nTransparency);
		for (uint32_t i = 0; i < Header._nTransparency; ++i)
		{
			Transparencies[i].init(fileStart, GlobalSequences.data(), (uint32_t)GlobalSequences.size(), t[i]);
		}
	}

	if (Header._nTransLookup > 0)
	{
		const int16_t* t = (int16_t*)(&fileStart[Header._ofsTransLookup]);
		TransparencyLookups.resize(Header._nTransLookup);
		memcpy(TransparencyLookups.data(), t, sizeof(int16_t) * Header._nTransLookup);
	}
}

void wowM2File::loadTextureAnimation(const uint8_t* fileStart)
{
	if (Header._nTextureanimations > 0)
	{
		const M2::texanimDef* t = (M2::texanimDef*)(&fileStart[Header._ofsTextureanimations]);
		TextureAnimations.resize(Header._nTextureanimations);
		for (uint32_t i = 0; i < Header._nTextureanimations; ++i)
		{
			TextureAnimations[i].init(fileStart, GlobalSequences.data(), (uint32_t)GlobalSequences.size(), t[i]);
		}
	}

	if (Header._nTexAnimLookup > 0)
	{
		const int16_t* t = (int16_t*)(&fileStart[Header._ofsTexAnimLookup]);
		TextureAnimationLookups.resize(Header._nTexAnimLookup);
		memcpy(TextureAnimationLookups.data(), t, sizeof(int16_t) * Header._nTexAnimLookup);
	}
}

void wowM2File::loadBones(const uint8_t* fileStart)
{
	if (Header._nAnimations == 0)
		return;


}

void wowM2File::loadRenderFlags(const uint8_t* fileStart)
{
	if (Header._nRenderFlags > 0)
	{
		const M2::renderflag* rfs = (M2::renderflag*)(&fileStart[Header._ofsRenderFlags]);
		RenderFlags.resize(Header._nRenderFlags);
		memcpy(RenderFlags.data(), rfs, sizeof(M2::renderflag) * Header._nRenderFlags);
	}
}

void wowM2File::loadAttachments(const uint8_t* fileStart)
{
	if (Header._nAttachments > 0)
	{
		Attachments.resize(Header._nAttachments);
		const M2::attach* att = (M2::attach*)(&fileStart[Header._ofsAttachments]);
		for (uint32_t i = 0; i < Header._nAttachments; ++i)
		{
			Attachments[i].id = att[i]._Id;
			Attachments[i].position = M2::fixCoordinate(att[i]._Position);
			Attachments[i].boneIndex = (att[i]._Bone >= 0 && att[i]._Bone < (int32_t)Header._nBones) ?
				att[i]._Bone : -1;
		}
	}

	if (Header._nAttachLookup > 0)
	{
		const int16_t* p = (int16_t*)(&fileStart[Header._ofsAttachLookup]);
		AttachLookups.resize(Header._nAttachLookup);
		memcpy(AttachLookups.data(), p, sizeof(int16_t)* Header._nAttachLookup);
	}
}

void wowM2File::loadParticleSystems(const uint8_t* fileStart)
{

}

void wowM2File::loadRibbonEmitters(const uint8_t* fileStart)
{

}

bool wowM2File::loadSkin(int index)
{
	if (index >= (int)SkinFileIDs.size())
		return false;

	CMemFile* file = WowEnvironment->openFileById(SkinFileIDs[index]);
	if (!file)
		return false;

	if (!SkinFile.loadFile(file))
	{
		delete file;
		return false;
	}

	delete file;
	return true;
}

wowSkinFile::wowSkinFile(const wowM2File* m2)
	: M2File(m2)
{

}

wowSkinFile::~wowSkinFile()
{

}

bool wowSkinFile::loadFile(CMemFile* file)
{
	const uint8_t* skinBuffer = file->getBuffer();

	const M2::skin_header* skinHeader = (const M2::skin_header*)skinBuffer;

	const int numGeosets = skinHeader->_nSubmeshes;
	const int numTexUnits = skinHeader->_nTextureUnits;
	const int numIndices = skinHeader->_nTriangles;

	if (numGeosets == 0 || numTexUnits == 0 || numIndices == 0)
		return true;

	//indices
	const uint16_t* indexLookup = (const uint16_t*)(&skinBuffer[skinHeader->_ofsIndices]);
	const uint16_t* triangles = (const uint16_t*)(&skinBuffer[skinHeader->_ofsTriangles]);

	Indices.resize(numIndices);
	for (uint16_t i = 0; i < numIndices; ++i)
	{
		Indices[i] = indexLookup[triangles[i]];
	}

	//geosets
	Geosets.resize(numGeosets);

	//texture unit
	const uint32_t numTexAnimLookup = (uint32_t)M2File->TextureAnimationLookups.size();
	const uint32_t numTexLookup = (uint32_t)M2File->TextureAnimationLookups.size();
	const uint32_t numRenderFlag = (uint32_t)M2File->RenderFlags.size();
	const uint32_t numColor = (uint32_t)M2File->Colors.size();
	const uint32_t numTransparencyLookup = (uint32_t)M2File->TransparencyLookups.size();
	const uint32_t numTextureFlag = (uint32_t)M2File->TextureFlags.size();

	const M2::textureUnit* t = (M2::textureUnit*)(&skinBuffer[skinHeader->_ofsTextureUnits]);
	for (int i = 0; i < numTexUnits; ++i)
	{
		SGeoset* geo = &Geosets[t[i]._submeshIdx];
		SGeoset::STexUnit texUnit;

		texUnit.Mode = t[i]._mode;
		texUnit.Shading = t[i]._shading;
		texUnit.TexID = (t[i]._textureIdx >= 0 && t[i]._textureIdx < (int16_t)numTexLookup) ?
			M2File->TexLookups[t[i]._textureIdx] : -1;

		texUnit.rfIndex = (t[i]._renderFlagsIdx >= 0 && t[i]._renderFlagsIdx < (int16_t)numRenderFlag) ?
			t[i]._renderFlagsIdx : -1;
		texUnit.ColorIndex = (t[i]._colorIdx >= 0 && t[i]._colorIdx < (int16_t)numColor) ?
			t[i]._colorIdx : -1;
		texUnit.TransIndex = (t[i]._transparencyIdx >= 0 && t[i]._transparencyIdx < (int16_t)numTransparencyLookup) ?
			M2File->TransparencyLookups[t[i]._transparencyIdx] : -1;
		texUnit.TexAnimIndex = (t[i]._animationIdx >= 0 && t[i]._animationIdx < (int16_t)numTexAnimLookup) ?
			M2File->TextureAnimationLookups[t[i]._animationIdx] : -1;
		texUnit.TexFlags = (texUnit.TexID >= 0 && texUnit.TexID < (int16_t)numTextureFlag)  ? M2File->TextureFlags[texUnit.TexID] : 0;
	
		geo->TextureUnits.push_back(texUnit);
	}

	return true;
}
