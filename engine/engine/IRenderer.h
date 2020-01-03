#pragma once

#include "aabbox3d.h"
#include "SMaterial.h"
#include "RenderStruct.h"
#include "SGlobalLayerData.h"

class ISceneNode;

class IRenderer
{
public:
	explicit IRenderer(ISceneNode* sceneNode) : SceneNode(sceneNode), Active(true) {}
	virtual ~IRenderer() = default;

public:
	virtual aabbox3df getBoundingBox() const = 0;

public:
	const SMaterial* getMaterialPtr() const { return &Material; }
	const SMaterial& getMaterial() const { return Material; }
	SMaterial& getMaterial() { return Material; }
	matrix4 getLocalToWorldMatrix() const;
	ISceneNode* getSceneNode() const { return SceneNode; }

	int getRenderQueue() const { return Material.RenderQueue; }
	void setGlobalLayerData(const SGlobalLayerData& layerData) { GlobalLayerData = layerData; }
	const SGlobalLayerData& getGlobalLayerData() const { return GlobalLayerData; }

public:
	bool Active;

protected:
	ISceneNode*		SceneNode;
	SMaterial	Material;
	SGlobalLayerData	GlobalLayerData;
};