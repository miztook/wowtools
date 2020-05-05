#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include "wowHeader.h"
#include "wowM2Struct.h"
#include "S3DVertex.h"
#include "wowAnimation.h"

class wowEnvironment;
class GameFile;
class CMemFile;

#define	ANIMATION_HANDSCLOSED	15

#define	RENDERFLAGS_UNLIT		1
#define	RENDERFLAGS_TWOSIDED	4
#define	RENDERFLAGS_BILLBOARD	8
#define	RENDERFLAGS_UNZWRITE		16
#define RENDERFLAGS_ADDCOLOR		256

enum E_BONE_TYPE
{
	EBT_NONE = 0,
	EBT_ROOT,
	EBT_LEFTHAND,
	EBT_RIGHTHAND,

	EBT_COUNT
};

class wowM2File
{
public:
	explicit wowM2File(const wowEnvironment* wowEnv);
	~wowM2File();

public:
	bool loadFile(const char* filename);

public:
	struct STexDef 
	{
		uint32_t	flags;
		ETextureTypes  type;
		std::string		filename;
	};

	struct SModelColor
	{
		SWowAnimation<vector3df>	colorAnim;
		SWowAnimationShort		opacityAnim;

		void init(const uint8_t* filedata, int32_t* globalSeq, uint32_t numGlobalSeq, const M2::colorDef& cd)
		{
			colorAnim.init(&cd.color, filedata, globalSeq, numGlobalSeq);
			opacityAnim.init(&cd.opacity, filedata, globalSeq, numGlobalSeq);
		}
	};

	struct SModelTransparency
	{
		SWowAnimation<uint16_t>	tranAnim;

		void init(const uint8_t* filedata, int32_t* globalSeq, uint32_t numGlobalSeq, const M2::transDef& td)
		{
			tranAnim.init(&td.trans, filedata, globalSeq, numGlobalSeq);
		}
	};

	struct SModelTextureAnim
	{
		SWowAnimation<vector3df>	trans, rot, scale;

		void init(const uint8_t* filedata, int32_t* globalSeq, uint32_t numGlobalSeq, const M2::texanimDef& t)
		{
			trans.init(&t.trans, filedata, globalSeq, numGlobalSeq);
			rot.init(&t.rot, filedata, globalSeq, numGlobalSeq);
			scale.init(&t.scale, filedata, globalSeq, numGlobalSeq);
		}
	};

	struct SModelAttachment			//挂点
	{
		SModelAttachment() : id(0), boneIndex(-1) {}
		int32_t id;
		int32_t boneIndex;
		vector3df position;
	};

	struct SModelAnimation
	{
		uint32_t	animID;
		uint32_t	animSubID;
		uint32_t	timeLength;

		int16_t		NextAnimation;
		int16_t		Index;
	};

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
	
	std::vector<int16_t>	TexLookups;
	std::vector<STexDef>	TexDefs;
	std::vector<int32_t>	GlobalSequences;
	std::vector<SModelColor>	Colors;
	std::vector<SModelTransparency>		Transparencies;
	std::vector<int16_t>	TransparencyLookups;
	std::vector<SModelTextureAnim>		TextureAnimations;

	std::vector<SModelAnimation>	Animations;
	std::vector<int16_t>	AnimationLookups;

private:
	void loadVertices(const uint8_t* fileStart);

	void loadBounds(const uint8_t* fileStart);

	void loadTextures(const uint8_t* fileStart);

	void loadSequences(const uint8_t* fileStart);

	void loadColor(const uint8_t* fileStart);

	void loadTransparency(const uint8_t* fileStart);

	void loadTextureAnimation(const uint8_t* fileStart);

	void loadBones(const uint8_t* fileStart);

	void loadRenderFlags(const uint8_t* fileStart);

	void loadAttachments(const uint8_t* fileStart);

	void loadParticleSystems(const uint8_t* fileStart);

	void loadRibbonEmitters(const uint8_t* fileStart);

	bool loadSkin(int index);

private:
	const wowEnvironment* WowEnvironment;
};