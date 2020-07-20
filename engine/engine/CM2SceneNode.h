#pragma once

#include "ISceneNode.h"
#include "aabbox3d.h"
#include "IRenderer.h"

class CM2SceneNode;
class wowM2File;
class wowSkinFile;

class CM2Renderer : public IRenderer
{
public:
	explicit CM2Renderer(CM2SceneNode* node);

public:
	aabbox3df getBoundingBox() const override;

private:
	aabbox3df Box;

	const wowM2File*	M2File;
	const wowSkinFile*		WowSkinFile;
};

class CM2SceneNode : public ISceneNode
{
public:
	explicit CM2SceneNode(std::shared_ptr<wowM2File> file);
	~CM2SceneNode();

public:
	struct SCharTexturePart
	{
		std::string name;
		int region;
		int layer;

		bool operator<(const SCharTexturePart& c) const
		{
			return layer < c.layer;
		}
	};

	struct SCharTexture
	{
		std::vector<SCharTexturePart>	textureParts;
	};

	struct SDynGeoset
	{

	};

	struct SDynBone
	{

	};

public:
	std::vector<SDynGeoset>		DynGeosets;
	std::vector<SDynBone>	DynBones;
	const wowSkinFile*		WowSkinFile;

public:
	std::list<SRenderUnit*> render(const IRenderer* renderer, const CCamera* cam) override;

private:
	std::shared_ptr<wowM2File> M2File;

	CM2Renderer	M2Renderer;


	friend class CM2Renderer;
};
