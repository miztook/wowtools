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
		: m_Transform(this), m_Enabled(true)
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

public:
	bool m_Enabled;

protected:
	CTransform		m_Transform;
	std::list<IRenderer*>	m_RendererList;
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