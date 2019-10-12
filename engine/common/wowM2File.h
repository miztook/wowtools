#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include "wowHeader.h"
#include "wowM2Struct.h"
#include "S3DVertex.h"

class wowEnvironment;
class GameFile;

class wowM2File
{
public:
	explicit wowM2File(const wowEnvironment* wowEnv);
	~wowM2File();

public:
	bool loadFile(const char* filename);

public:
	std::string		Name;
	std::string		Dir;
	std::string		FileName;
	M2Type		Type;
	
	M2::Header	Header;

	//
	uint32_t		NumBoundingVerts;
	uint32_t		NumBoundingTriangles;
	uint32_t		NumTextures;
	uint32_t		NumTexLookup;
	uint32_t		NumAttachments;
	uint32_t		NumAttachLookup;
	uint32_t		NumGlobalSequences;
	uint32_t		NumColors;
	uint32_t		NumTransparencies;
	uint32_t		NumTranparencyLookukp;
	uint32_t		NumTexAnim;
	uint32_t		NumAnimations;
	uint32_t		NumAnimationLookup;
	uint32_t		NumBones;
	uint32_t		NumBoneLookup;
	uint32_t		NumRenderFlags;
	uint32_t		NumParticleSystems;
	uint32_t		NumRibbonEmitters;
	uint32_t		NumModelCameras;

	aabbox3df		CollisionAABBox;			//和周围的潜在场景碰撞
	float		CollisionRadius;
	aabbox3df	BoundingAABBox;			//自身的包围
	float		BoundingRadius;

	std::vector<SVertex_PNT2WA>		Vertices;
	std::vector<uint32_t>	SkinFileIDs;
	std::vector<uint32_t>	GlobalSequences;


private:
	void loadVertices();

	void loadBounds();

	void loadTextures();

	void loadSequences();

	void loadColor();

	void loadTransparency();

	void loadTextureAnimation();

	void loadBones();

private:
	const wowEnvironment* WowEnvironment;
};