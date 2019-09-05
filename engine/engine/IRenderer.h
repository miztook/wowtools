#pragma once

#include "aabbox3d.h"
#include "SMaterial.h"
#include "RenderStruct.h"

class ISceneNode;

class IRenderer
{
public:
	IRenderer(const ISceneNode* sceneNode) : SceneNode(sceneNode), Active(true) {}
	virtual ~IRenderer() = default;

public:
	virtual aabbox3df getBoundingBox() const = 0;

public:
	SMaterial& getMaterial() { return Material; }
	matrix4 getLocalToWorldMatrix() const;

public:
	bool Active;

protected:
	const ISceneNode*		SceneNode;
	SMaterial	Material;
};