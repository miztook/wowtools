#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include "vector3d.h"
#include "quaternion.h"
#include "S3DVertex.h"
#include "aabbox3d.h"
#include "wowWMOStruct.h"

class CMemFile;
class wowEnvironment;

enum class E_WMO_SHADER : int32_t
{
	Diffuse = 0,
	Specular,
	Metal,
	Env,
	Opaque,
	EnvMetal,
	TwoLayerDiffuse,
	TwoLayerEnvMetal,
	TwoLayerTerrain,
	DiffuseEmissive,
	Diffuse10,		//
	MaskedEnvMetal,
	EnvMetalEmissive,
	TwoLayerDiffuseOpaque,
	TwoLayerDiffuseEmissive,
	Diffuse15,		//
	Diffuse16,		//
};

struct SWMOMaterial
{
	SWMOMaterial()
	{
		flags = 0;
		alphatest = false;
		shaderType = E_WMO_SHADER::Diffuse;
	}
	uint32_t flags;
	E_WMO_SHADER	shaderType;
	uint32_t tex1FileId;
	uint32_t tex2FileId;
	SColor color0;
	SColor color1;
	SColor color2;
	bool alphatest;
};

struct SWMOPortal
{
	SWMOPortal() : vStart(0), vCount(0), frontGroupIndex(-1), backGroupIndex(-1) {}

	uint16_t vStart;
	uint16_t vCount;
	int16_t frontGroupIndex;
	int16_t backGroupIndex;
	plane3df	plane;
	aabbox3df	 box;

	bool isValid() const { return frontGroupIndex != -1 && backGroupIndex != -1; }
};

struct SWMOPortalRelation
{
	uint16_t portalIndex;
	int16_t groupIndex;
	bool face;
};

struct SWMOLight
{
	E_LIGHT_TYPE lighttype;
	SColor color;
	vector3df position;
	float intensity;
	float attenStart, attenEnd;
};

struct SWMODoodadSet
{
	char name[20];
	uint32_t start;
	uint32_t count;
};

struct SWMODoodad
{
	char name[256];
	vector3df position;
	quaternion quat;
	float scale;
	SColor color;
};

struct SWMOFog
{
	vector3df position;
	float radius1, radius2;
	float fogend, fogstart;
	SColor color;
};

struct SWMOBatch
{
	uint32_t indexStart;
	uint16_t indexCount;
	uint16_t vertexStart;
	uint16_t vertexEnd;
	uint16_t matId;

	aabbox3df box;
	uint16_t getVertexCount() const { return vertexEnd - vertexStart + 1; }
};

struct SWMOBspNode
{
	uint16_t planetype;		//4: leaf, 0 for YZ-plane, 1 for XZ-plane, 2 for XY-plane?
	int16_t left;
	int16_t right;
	uint16_t numfaces;
	uint16_t firstface;
	float distance;
	uint16_t startIndex;
	uint16_t minVertex;
	uint16_t maxVertex;
};

struct SWMOGroup
{
	uint32_t		index;
	uint32_t		flags;
	aabbox3df		box;
	bool	hasVertexColor;
	std::string			name;
	std::vector<SWMOBatch>	batchList;
	std::vector<uint16_t>   indices;
	std::vector<SVertex_PNCT2>	vertices;
	std::vector<uint16_t>	lightList;
	std::vector<uint16_t>	doodadList;
};

class wowWMOFile
{
public:
	explicit wowWMOFile(const wowEnvironment* wowEnv);
	~wowWMOFile();

public:
	bool loadFile(const char* filename);
	bool loadGroupFile(CMemFile* pMemFile, SWMOGroup& group);

public:
	WMO::wmoHeader		Header;
	aabbox3df	Box;

	std::string Name;

	std::vector<SWMOMaterial>	MaterialList;
	std::vector<SWMOGroup>		GroupList;
	std::vector<SWMOLight>		LightList;
	std::vector<SWMODoodadSet>	DoodadSets;
	std::vector<SWMOFog>	FogList;

	std::vector<char>		TextureFileNameBlock;
	std::vector<char>		GroupNameBlock;

private:
	const wowEnvironment* WowEnvironment;
};