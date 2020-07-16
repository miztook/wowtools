#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include <set>
#include <map>
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

class wowM2File;


class SGeoset
{
public:
	struct SBoneUnit
	{
		uint32_t		BoneVStart = 0;				//在skin bone顶点中的偏移
		uint16_t		TCount = 0;
		uint16_t		StartIndex = 0;				//在skin 中index偏移
		uint8_t		BoneCount = 0;
		int8_t		Index = -1;

		std::set<uint8_t>			BoneIndices;
		std::map<uint8_t, uint8_t>	local2globalMap;					//local -> global
	};

	struct STexUnit
	{
		int16_t	TexID;				//使用纹理编号
		int16_t	rfIndex;				//render flag索引
		int16_t	ColorIndex;
		int16_t  TransIndex;
		int16_t	TexAnimIndex;
		uint16_t	Mode;
		uint16_t	Shading;		//shader?
		uint32_t  TexFlags;

		bool WrapX() const { return TexAnimIndex == -1 && (TexFlags & TEXTURE_WRAPX) == 0; }
		bool WrapY() const { return TexAnimIndex == -1 && (TexFlags & TEXTURE_WRAPY) == 0; }
	};

public:
	uint32_t		GeoID = 0;
	uint16_t		VStart = 0;
	uint16_t		VCount = 0;
	uint16_t		IStart = 0;
	uint16_t		ICount = 0;
	uint16_t		MaxWeights = 0;
	
	std::vector<STexUnit>	TextureUnits;
	std::vector<SBoneUnit>	BoneUnits;
};

class wowSkinFile
{
public:
	explicit wowSkinFile(const wowM2File* m2);
	~wowSkinFile();

	bool loadFile(CMemFile* file);

public:
	std::vector<uint16_t>	Indices;
	std::vector<SGeoset>	Geosets;

private:
	const wowM2File*	M2File;
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

	struct SRenderFlag
	{
		//raw
		uint16_t	flags;
		uint16_t	blend;

		bool lighting() const { return (flags & RENDERFLAGS_UNLIT) == 0; }
		bool zwrite() const { return (flags & RENDERFLAGS_UNZWRITE) == 0; }
		bool frontCulling() const { return (flags & RENDERFLAGS_TWOSIDED) == 0; }
		bool invisible() const { return (flags & 256) != 0; }
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
	std::vector<int16_t>	TextureAnimationLookups;
	std::vector<SRenderFlag>	RenderFlags;

	std::vector<uint32_t>	TextureFlags;
	std::vector<ETextureTypes>	TextureTypes;

	std::vector<SModelAnimation>	Animations;
	std::vector<int16_t>	AnimationLookups;

	std::vector<SModelAttachment>	Attachments;
	std::vector<int16_t>	AttachLookups;

	wowSkinFile	SkinFile;

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