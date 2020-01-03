#pragma once

#include "matrix4.h"
#include "quaternion.h"
#include "function3d.h"
#include <list>

class ISceneNode;

class CTransform
{
public:
	using ON_TRANSFORM_CHANGED = void(*)(const CTransform* transform);
	using ON_HIEARCHY_CHANGED = void(*)(const CTransform* transform);

public:
	explicit CTransform(ISceneNode* node)
	{
		setDirty();
		SceneNode = node;
		Parent = nullptr;
	}

	~CTransform()
	{
		deleteAllChildren();

		ASSERT(CallbackTransformChangedList.empty());
		ASSERT(CallbackHiearchyChangedList.empty());
		ASSERT(ChildList.empty());
	}

public:
	ISceneNode* getSceneNode() const { return SceneNode; }
	void setSceneNode(ISceneNode* node) { SceneNode = node; }

	const matrix4& getRelativeRotateMatrix() const { return RelativeRotateMatrix; }
	const matrix4& getRelativeTransformation() const { return RelativeTransformation; }

	const matrix4& getAbsoluteRotateMatrix() const;
	const matrix4& getAbsoluteTransformation() const;
	
	void setRelativeTransformation(const matrix4& mat);

	//
	vector3df getDir() const 
	{ 
		vector4df v = RelativeRotateMatrix.getRow(2);
		return vector3df(v.x, v.y, v.z);
	}
	vector3df getUp() const 
	{
		vector4df v = RelativeRotateMatrix.getRow(1);
		return vector3df(v.x, v.y, v.z);
	}
	vector3df getRight() const
	{
		vector4df v = RelativeRotateMatrix.getRow(0);
		return vector3df(v.x, v.y, v.z);
	}
	vector3df getPos() const { return RelativeTransformation.getTranslation(); }
	vector3df getScale() const { return RelativeTransformation.getScale(); }

	void setDirAndUp(const vector3df& dir, const vector3df& up);
	void setPos(const vector3df& pos) { RelativeTransformation.setTranslation(pos); }
	void setScale(const vector3df& scale);
	void rotate(const quaternion& q);
	void setPosDirUp(const vector3df& pos, const vector3df& dir, const vector3df& up);
	void setPosDirUpScale(const vector3df& pos, const vector3df& dir, const vector3df& up, const vector3df& scale);

	void setDirty();

	//
	void setParent(CTransform* parent);
	CTransform* getParent() const { return Parent; }

	void addChild(CTransform* child);
	bool removeChild(CTransform* child);
	bool hasChild() const { return !ChildList.empty(); }
	void deleteAllChildren();
	const std::list<CTransform*>& getChildList() const { return ChildList; }

	//
	void RegisterTransformChangedCallback(ON_TRANSFORM_CHANGED callback)
	{
		CallbackTransformChangedList.push_back(callback);
	}

	void UnregisterTransformChangedCallback(ON_TRANSFORM_CHANGED callback)
	{
		CallbackHiearchyChangedList.remove(callback);
	}

	uint32_t getHiearchyCount() const
	{
		uint8_t n = 0;
		CTransform* p = Parent;
		while (p)
		{
			++n;
			p = p->Parent;
		}
		return n;
	}

private:
	void CheckTransformChange() const;

private:
	matrix4			RelativeRotateMatrix;
	matrix4			RelativeTransformation;
	mutable matrix4			AbsoluteRotateMatrix;
	mutable matrix4			AbsoluteTransformation;
	mutable bool		TransformDirty;

	ISceneNode*		SceneNode;

	CTransform* Parent;
	std::list<CTransform*>		ChildList;

	std::list<ON_TRANSFORM_CHANGED>		CallbackTransformChangedList;
	std::list<ON_HIEARCHY_CHANGED>		CallbackHiearchyChangedList;
};

inline void CTransform::setDirty()
{
	TransformDirty = true;
	for (CTransform* child : ChildList)
	{
		child->setDirty();
	}
}

inline void CTransform::setParent(CTransform* parent)
{
	if(Parent && Parent != parent)
		Parent->removeChild(this);

	Parent = parent;
	if (Parent)
		Parent->addChild(this);
}

inline void CTransform::addChild(CTransform* child)
{
	ASSERT(child && child != this);

	if (child->Parent && child->Parent != this)
		child->Parent->removeChild(this);

	ChildList.push_back(child);
	child->Parent = this;
}

inline bool CTransform::removeChild(CTransform* child)
{
	ASSERT(child && child != this);

	for (CTransform* trans : ChildList)
	{
		if (trans == child)
		{
			ChildList.remove(child);
			child->Parent = nullptr;
			return true;
		}
	}
	return false;
}

inline void CTransform::deleteAllChildren()
{
	for (CTransform* child : ChildList)
	{
		child->deleteAllChildren();
		delete child;
	}
	ChildList.clear();
}

inline void CTransform::CheckTransformChange() const
{
	if (TransformDirty)
	{
		if (Parent)
		{
			AbsoluteRotateMatrix = getRelativeRotateMatrix() * Parent->getAbsoluteRotateMatrix();
			AbsoluteTransformation = getRelativeTransformation() * Parent->getAbsoluteTransformation();
		}
		else
		{
			AbsoluteRotateMatrix = getRelativeRotateMatrix();
			AbsoluteTransformation = getRelativeTransformation();
		}
		for (const auto& cb : CallbackTransformChangedList)
		{
			cb(this);
		}
		TransformDirty = false;
	}
}

inline const matrix4& CTransform::getAbsoluteRotateMatrix() const
{
	CheckTransformChange();
	return AbsoluteRotateMatrix;
}

inline const matrix4& CTransform::getAbsoluteTransformation() const
{
	CheckTransformChange();
	return AbsoluteTransformation;
}

inline void CTransform::setRelativeTransformation(const matrix4& mat)
{
	RelativeRotateMatrix = mat;
	RelativeRotateMatrix.scale(vector3df::One());
	RelativeRotateMatrix.setTranslation(vector3df::Zero());

	RelativeTransformation = mat;
}

inline void CTransform::setDirAndUp(const vector3df& dir, const vector3df& up)
{
	RelativeRotateMatrix = f3d::transformMatrix(dir, up, vector3df::Zero());
	matrix4 mat = RelativeRotateMatrix;
	mat.scale(getScale());
	mat.setTranslation(getPos());

	RelativeTransformation = mat;
}

inline void CTransform::setScale(const vector3df& scale)
{
	matrix4 mat = RelativeRotateMatrix;
	mat.scale(scale);
	mat.setTranslation(getPos());

	RelativeTransformation = mat;
}

inline void CTransform::rotate(const quaternion& q)
{
	RelativeRotateMatrix = RelativeRotateMatrix * q.toMatrix();
	matrix4 mat = RelativeRotateMatrix;
	mat.scale(getScale());
	mat.setTranslation(getPos());

	RelativeTransformation = mat;
}

inline void CTransform::setPosDirUp(const vector3df& pos, const vector3df& dir, const vector3df& up)
{
	RelativeRotateMatrix = f3d::transformMatrix(dir, up, vector3df::Zero());
	matrix4 mat = RelativeRotateMatrix;
	mat.scale(getScale());
	mat.setTranslation(pos);

	RelativeTransformation = mat;
}

inline void CTransform::setPosDirUpScale(const vector3df& pos, const vector3df& dir, const vector3df& up, const vector3df& scale)
{
	RelativeRotateMatrix = f3d::transformMatrix(dir, up, vector3df::Zero());
	matrix4 mat = RelativeRotateMatrix;
	mat.scale(scale);
	mat.setTranslation(pos);

	RelativeTransformation = mat;
}
