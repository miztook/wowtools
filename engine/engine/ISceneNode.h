#pragma once

#include "CTransform.h"
#include "vector3d.h"
#include "quaternion.h"
#include "aabbox3d.h"
#include <list>
#include <algorithm>
#include <functional>

class IRenderer;
class CCamera;
struct SRenderUnit;
using TRAVERSE_SCENENODE_FUNC = std::function<void(ISceneNode* node)>;

class ISceneNode
{
public:
	ISceneNode(ISceneNode* parent)
		: m_Transform(this), m_Active(true), m_ToDelete(false)
	{
		if (parent)
			getTransform()->setParent(parent->getTransform());
	}

	virtual ~ISceneNode() = default;

public:
	virtual void tick(uint32_t tickTime) {}
	virtual SRenderUnit* render(const IRenderer* renderer, const CCamera* cam) = 0;

public:
	void traverse(TRAVERSE_SCENENODE_FUNC func);
	CTransform* getTransform() { return &m_Transform; }
	const CTransform* getTransform() const { return &m_Transform; }
	const std::list<IRenderer*>& getRendererList() const { return m_RendererList; }
	void markDelete() { m_ToDelete = true; }
	bool isToDelete() const { return m_ToDelete; }
	static void checkDelete(ISceneNode* node);
	void setActive(bool active) { m_Active = active; }
	bool activeSelf() const { return m_Active; }

protected:
	CTransform		m_Transform;
	std::list<IRenderer*>	m_RendererList;
	bool m_ToDelete;
	bool m_Active;
};

inline void ISceneNode::traverse(TRAVERSE_SCENENODE_FUNC func)
{
	func(this);

	for (const auto& trans : m_Transform.getChildList())
	{
		ISceneNode* node = trans->getSceneNode();
		node->traverse(func);
	}
}

inline void ISceneNode::checkDelete(ISceneNode* node)
{
	if (!node->isToDelete())
		return;
	for (const auto& trans : node->getTransform()->getChildList())
	{
		ISceneNode* node = trans->getSceneNode();
		if (node->isToDelete())
			checkDelete(node);
	}
	delete node;
}