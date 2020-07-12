#pragma once

#include "ISceneNode.h"
#include "aabbox3d.h"
#include "IRenderer.h"

class CMesh;
class wowM2File;

class CMeshRenderer : public IRenderer
{
public:
	explicit CMeshRenderer(const CMesh* mesh, ISceneNode* node);

public:
	aabbox3df getBoundingBox() const override;
	const CMesh* getMesh() const { return Mesh; }

private:
	const CMesh* Mesh;
	aabbox3df Box;
};

class CM2SceneNode : public ISceneNode
{
public:
	explicit CM2SceneNode(std::shared_ptr<wowM2File> file);
	~CM2SceneNode();

public:
	IRenderer* getMeshRenderer() const;

public:
	SRenderUnit* render(const IRenderer* renderer, const CCamera* cam) override;

private:
	std::shared_ptr<wowM2File> M2File;
};
