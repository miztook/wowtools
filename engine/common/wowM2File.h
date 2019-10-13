#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include "wowHeader.h"
#include "wowM2Struct.h"
#include "S3DVertex.h"

class wowEnvironment;
class GameFile;
class CMemFile;

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
	aabbox3df		CollisionAABBox;			//和周围的潜在场景碰撞
	float		CollisionRadius;
	aabbox3df	BoundingAABBox;			//自身的包围
	float		BoundingRadius;
	aabbox3df		BoundingBox;

	std::vector<SVertex_PNT2WA>		Vertices;
	std::vector<uint32_t>	SkinFileIDs;
	std::vector<uint32_t>	GlobalSequences;


private:
	void loadVertices(const uint8_t* fileStart);

	void loadBounds(const uint8_t* fileStart);

	void loadTextures(const uint8_t* fileStart);

	void loadSequences(const uint8_t* fileStart);

	void loadColor(const uint8_t* fileStart);

	void loadTransparency(const uint8_t* fileStart);

	void loadTextureAnimation(const uint8_t* fileStart);

	void loadBones(const uint8_t* fileStart);

private:
	const wowEnvironment* WowEnvironment;
};