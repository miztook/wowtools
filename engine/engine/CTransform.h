#pragma once

#include "matrix4.h"
#include "quaternion.h"
#include "function3d.h"

class CTransform
{
public:
	const matrix4& getAbsoluteRotateMatrix() const { return AbsoluteRotateMatrix; }
	const matrix4& getRelativeRotateMatrix() const { return RelativeRotateMatrix; }

	const matrix4& getAbsoluteTransformation() const { return AbsoluteTransformation; }
	const matrix4& getRelativeTransformation() const { return RelativeTransformation; }
	void setRelativeTransformation(const matrix4& mat);

	//
	matrix4 getDir() const { return f3d::normalize(RelativeRotateMatrix.getRow(2)); }
	matrix4 getUp() const { return f3d::normalize(RelativeRotateMatrix.getRow(1)); }
	matrix4 getRight() const { return f3d::normalize(RelativeRotateMatrix.getRow(0)); }
	void setDirAndUp(const vector3df& dir, const vector3df& up);
	vector3df getPos() const { return RelativeTransformation.getTranslation(); }
	void setPos(const vector3df& pos) { RelativeTransformation.setTranslation(pos); }
	vector3df getScale() const { return RelativeTransformation.getScale(); }
	void setScale(const vector3df& scale);

	vector3df getAbsDir() const { return f3d::normalize(AbsoluteRotateMatrix.getRow(2)); }
	vector3df getAbsUp() const { return f3d::normalize(AbsoluteRotateMatrix.getRow(1)); }
	vector3df getAbsRight() const { return f3d::normalize(AbsoluteRotateMatrix.getRow(0)); }
	vector3df getAbsPos() const { return AbsoluteTransformation.getTranslation(); }
	vector3df getAbsScale() const { return AbsoluteTransformation.getScale(); }

	void update(const CTransform* parent);
	void rotate(const quaternion& q);

private:
	matrix4			RelativeRotateMatrix;
	matrix4			RelativeTransformation;
	matrix4			AbsoluteRotateMatrix;
	matrix4			AbsoluteTransformation;
};

inline void CTransform::setRelativeTransformation(const matrix4& mat)
{
	vector3df s = mat.getScale();
	RelativeRotateMatrix = mat;
	RelativeRotateMatrix.setScale(vector3df(1.0f / s.x, 1.0f / s.y, 1.0f / s.z));
	RelativeRotateMatrix.setTranslation(vector3df::Zero());

	RelativeTransformation = mat;
}

inline void CTransform::setDirAndUp(const vector3df& dir, const vector3df& up)
{
	RelativeRotateMatrix = f3d::transformMatrix(dir, up, vector3df::Zero());
	matrix4 mat = RelativeRotateMatrix;
	mat.setScale(getScale());
	mat.setTranslation(getPos());

	RelativeTransformation = mat;
}

inline void CTransform::setScale(const vector3df& scale)
{
	matrix4 mat = RelativeRotateMatrix;
	mat.setScale(scale);
	mat.setTranslation(getPos());

	RelativeTransformation = mat;
}

inline void CTransform::update(const CTransform* parent)
{
	if (parent)
	{
		AbsoluteRotateMatrix = getRelativeRotateMatrix() * parent->getAbsoluteRotateMatrix();
		AbsoluteTransformation = getRelativeTransformation() * parent->getAbsoluteTransformation();
	}
	else
	{
		AbsoluteRotateMatrix = getRelativeRotateMatrix();
		AbsoluteTransformation = getRelativeTransformation();
	}
}

inline void CTransform::rotate(const quaternion& q)
{
	RelativeRotateMatrix = q * RelativeRotateMatrix;
	matrix4 mat = RelativeRotateMatrix;
	mat.setScale(getScale());
	mat.setTranslation(getPos());

	RelativeTransformation = mat;
}
