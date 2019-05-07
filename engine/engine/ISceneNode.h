#pragma once

#include "CTransform.h"
#include "vector3d.h"
#include "quaternion.h"
#include "aabbox3d.h"
#include <list>
#include <algorithm>

class CRenderScene;

class ISceneNode
{
public:
	ISceneNode(ISceneNode* parent, const CRenderScene* renderScene)
		: Distance(0), RenderScene(renderScene)
	{
		setParent(parent);
		if (Parent)
			Parent->addChild(this);

		NeedUpdate = true;
		Visible = true;
	}

	virtual ~ISceneNode() = default;

public:
	virtual void registerSceneNode(bool frustumcheck);
	virtual void tick(uint32_t tickTime, bool visible) {}
	virtual void render() = 0;
	virtual aabbox3df getBoundingBox() const = 0;
	virtual bool isNodeEligible() const = 0;
	virtual void onUpdated() { updateAABB(); }

public:
	const matrix4& getAbsoluteRotateMatrix() const { return m_Transform.getAbsoluteRotateMatrix(); }
	const matrix4& getRelativeRotateMatrix() const { return m_Transform.getRelativeRotateMatrix(); }

	const matrix4& getAbsoluteTransformation() const { return m_Transform.getAbsoluteTransformation(); }
	const matrix4& getRelativeTransformation() const { return m_Transform.getRelativeTransformation(); }
	void setRelativeTransformation(const matrix4& mat)
	{
		m_Transform.setRelativeTransformation(mat);
		NeedUpdate = true;
	}

	//
	vector3df getDir() const { return m_Transform.getDir(); }
	vector3df getUp() const { return m_Transform.getUp(); }
	void setDir(const vector3df& dir) { setDirAndUp(dir, getUp()); }
	void setUp(const vector3df& up) { setDirAndUp(getDir(), up); }
	void setDirAndUp(const vector3df& dir, const vector3df& up)
	{
		m_Transform.setDirAndUp(dir, up);
		NeedUpdate = true;
	}

	vector3df getPos() const { return m_Transform.getPos(); }
	void setPos(const vector3df& pos) { m_Transform.setPos(pos); NeedUpdate = true; }
	vector3df getScale() const { return m_Transform.getScale(); }
	void setScale(const vector3df& scale) { m_Transform.setScale(scale); NeedUpdate = true; }
	void setScale(float scale) { return setScale(vector3df(scale, scale, scale)); }

	vector3df getAbsDir() const { return m_Transform.getAbsDir(); }
	vector3df getAbsUp() const { return m_Transform.getAbsUp(); }
	vector3df getAbsPos() const { return m_Transform.getAbsPos(); }
	vector3df getAbsScale() const { return m_Transform.getAbsScale(); }

	void rotateAxisY(float radians)
	{
		quaternion q(vector3df::UnitY(), radians);
		rotate(q);
	}
	void move(const vector3df& offset)
	{
		setPos(getPos() + offset);
	}

	void update(bool includeChildren = true);
	void updateAABB();
	void addChild(ISceneNode* child);
	bool removeChild(ISceneNode* child);
	bool hasChild() const;
	void removeAllChildren(bool bIncludeChild = true);

	const aabbox3df& getWorldBoundingBox() const { return WorldBoundingBox; }

	void setParent(ISceneNode* parent) { Parent = parent; updateGeneration(); }
	uint8_t getGeneration() const { return Generation; }

	ISceneNode* getParent() const { return Parent; }
	float getDistance() const { return Distance; }

protected:
	void rotate(const quaternion& q) { m_Transform.rotate(q); NeedUpdate = true; }
	void updateGeneration();

protected:
	std::list<ISceneNode*>		ChildNodeList;			//child lists
	float  Distance;		//ºÍÉãÏñ»úµÄ¾àÀë

private:
	CTransform		m_Transform;
	aabbox3df			WorldBoundingBox;
	const CRenderScene*		RenderScene;
	ISceneNode*			Parent;
	uint8_t		Generation;

public:
	bool		NeedUpdate;
	bool		Visible;
};

inline void ISceneNode::update(bool includeChildren /*= true*/)
{
	if (NeedUpdate)
	{
		const CTransform* trans = Parent ? &Parent->m_Transform : nullptr;
		m_Transform.update(trans);

		//	ASSERT(!AbsoluteTransformation.equals(matrix4::Zero()));

		onUpdated();

		if (includeChildren)
		{
			for (ISceneNode* node : ChildNodeList)
			{
				node->NeedUpdate = true;
				node->update(includeChildren);
			}
		}
		NeedUpdate = false;
	}
}

inline void ISceneNode::updateAABB()
{
	WorldBoundingBox = getBoundingBox();
	if (WorldBoundingBox.isValid())
	{
		WorldBoundingBox = getAbsoluteTransformation().transformBox(WorldBoundingBox);
	}
}

inline void ISceneNode::addChild(ISceneNode* child)
{
	assert(child && child != this);

	if (child->Parent && child->Parent != this)
		child->Parent->removeChild(this);

	ChildNodeList.push_back(child);
	child->Parent = this;
}

inline bool ISceneNode::removeChild(ISceneNode* child)
{
	assert(child && child != this);

	for (ISceneNode* node : ChildNodeList)
	{
		if (node == child)
		{
			ChildNodeList.remove(child);
			child->Parent = nullptr;
			return true;
		}
	}

	return false;
}

inline void ISceneNode::removeAllChildren(bool bIncludeChild)
{
	for (ISceneNode* node : ChildNodeList)
	{
		if (bIncludeChild)
			node->removeAllChildren();
		node->Parent = nullptr;
		delete node;
	}
	ChildNodeList.clear();
}

inline void ISceneNode::updateGeneration()
{
	uint8_t n = 0;
	ISceneNode* p = Parent;
	while (p)
	{
		++n;
		p = p->Parent;
	}
	Generation = n;
}

inline bool ISceneNode::hasChild() const
{
	return !ChildNodeList.empty();
}

inline void ISceneNode::registerSceneNode(bool frustumcheck)
{
	for (ISceneNode* node : ChildNodeList)
	{
		node->registerSceneNode(frustumcheck);
	}
}