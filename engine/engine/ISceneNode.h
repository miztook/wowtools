#pragma once

#include "CTransform.h"
#include "vector3d.h"
#include "quaternion.h"
#include "aabbox3d.h"
#include <list>
#include <algorithm>

class IRenderer;

class ISceneNode
{
public:
	ISceneNode(ISceneNode* parent)
		: m_Transform(this), m_Enabled(true), m_ToDelete(false)
	{
		if (parent)
			getTransform()->setParent(parent->getTransform());
	}

	virtual ~ISceneNode() = default;

public:
	virtual void registerSceneNode();
	virtual void tick(uint32_t tickTime) {}
	virtual void render(IRenderer* renderer) = 0;

public:
	CTransform* getTransform() { return &m_Transform; }
	const CTransform* getTransform() const { return &m_Transform; }
	const std::list<IRenderer*>& getRendererList() const { return m_RendererList; }
	void markDelete() { m_ToDelete = true; }
	bool isToDelete() const { return m_ToDelete; }
	static void checkDelete(ISceneNode* node);

public:
	bool m_Enabled;
	
protected:
	CTransform		m_Transform;
	std::list<IRenderer*>	m_RendererList;
	bool m_ToDelete;
};

inline void ISceneNode::registerSceneNode()
{
	if (!m_Enabled)
		return;

	for (const auto& trans : m_Transform.getChildList())
	{
		ISceneNode* node = trans->getSceneNode();
		node->registerSceneNode();
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