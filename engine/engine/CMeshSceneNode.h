#pragma once

#include "ISceneNode.h"
#include "aabbox3d.h"
#include "IRenderer.h"

class CMesh;

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

class CMeshSceneNode : public ISceneNode
{
public:
	explicit CMeshSceneNode(ISceneNode* parent);
	~CMeshSceneNode() = default;

public:
	void addMesh(CMesh* pMesh);
	void removeMesh(CMesh* pMesh);

public:
	SRenderUnit* render(const IRenderer* renderer, const CCamera* cam) override;

private:

};
