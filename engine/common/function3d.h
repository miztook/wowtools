#pragma once

#include "matrix4.h"
#include "vector2d.h"
#include "vector3d.h"
#include "quaternion.h"

namespace f3d
{

inline vector3df normalize(const vector3df& v)
{
	float mag = v.magnitude();
	if (mag < 1e-12	&& mag > -1e-12)
		return vector3df(0.0f);
	else
		return v / mag;
}

inline float dotProduct(const vector3df& v1, const vector3df& v2)
{
	float fRet;
	fRet = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	return fRet;
}

inline vector3df crossProduct(const vector3df& v1, const vector3df& v2)
{
	return vector3df(v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x);
}

inline matrix4 transformMatrix(const vector3df& vecDir, const vector3df& vecUp, const vector3df& vecPos)
{
	matrix4   mat(false);
	vector3df   vecXAxis, vecYAxis, vecZAxis;

	vecZAxis = normalize(vecDir);
	vecYAxis = normalize(vecUp);
	vecXAxis = normalize(crossProduct(vecYAxis, vecZAxis));

	ASSERT(vecXAxis.magnitude() > 1e-3);

	mat._11 = vecXAxis.x;
	mat._12 = vecXAxis.y;
	mat._13 = vecXAxis.z;

	mat._21 = vecYAxis.x;
	mat._22 = vecYAxis.y;
	mat._23 = vecYAxis.z;

	mat._31 = vecZAxis.x;
	mat._32 = vecZAxis.y;
	mat._33 = vecZAxis.z;

	mat._41 = vecPos.x;
	mat._42 = vecPos.y;
	mat._43 = vecPos.z;
	mat._44 = 1.0f;

	return mat;
}

inline matrix4 rotateX(float vRad)
{
	matrix4 ret;
	ret._33 = ret._22 = (float)cos(vRad);
	ret._23 = (float)sin(vRad);
	ret._32 = (float)-ret._23;

	return ret;
}

inline matrix4 rotateY(float vRad)
{
	matrix4 ret;
	ret._33 = ret._11 = (float)cos(vRad);
	ret._31 = (float)sin(vRad);
	ret._13 = -ret._31;

	return ret;
}

inline matrix4 rotateZ(float vRad)
{
	matrix4 ret;
	ret._22 = ret._11 = (float)cos(vRad);
	ret._12 = (float)sin(vRad);
	ret._21 = -ret._12;

	return ret;
}

inline matrix4 translate(float x, float y, float z)
{
	matrix4 ret(true);
	ret.setTranslation(vector3df(x, y, z));
	return ret;
}

inline vector3df viewToWorld(const vector3df& vIn, const matrix4& matView)
{
	vector3df vOut;
	vOut.x = vIn.x * matView._11 + vIn.y * matView._12 + vIn.z * matView._13;
	vOut.y = vIn.x * matView._21 + vIn.y * matView._22 + vIn.z * matView._23;
	vOut.z = vIn.x * matView._31 + vIn.y * matView._32 + vIn.z * matView._33;
	return vOut;
}

inline matrix4 makeViewMatrix(const vector3df& from, const vector3df& dir, const vector3df& vecUp, float roll)
{
	matrix4 view;
	vector3df up, right, view_dir;

	view_dir = normalize(dir);
	right = crossProduct(vecUp, dir);
	right = normalize(right);
	up = crossProduct(dir, right);
	up = normalize(up);

	view._11 = right.x;
	view._21 = right.y;
	view._31 = right.z;
	view._12 = up.x;
	view._22 = up.y;
	view._32 = up.z;
	view._13 = view_dir.x;
	view._23 = view_dir.y;
	view._33 = view_dir.z;

	view._41 = -dotProduct(right, from);
	view._42 = -dotProduct(up, from);
	view._43 = -dotProduct(view_dir, from);

	// Set roll
	if (roll != 0.0f)
		view = rotateZ(-roll) * view;

	return view;
}

inline matrix4 makeOrthoOffCenterMatrixLH(float fLeft, float fRight, float fTop, float fBottom, float fZNear, float fZFar)
{
	matrix4 m(false);
	float fWidth = fRight - fLeft;
	float fHeight = fTop - fBottom;
	m._11 = 2.0f / fWidth;
	m._22 = 2.0f / fHeight;
	m._33 = 1.0f / (fZFar - fZNear);
	m._43 = -fZNear * m._33;
	m._44 = 1.0f;
	m._12 = m._13 = m._14 = 0.0f;
	m._21 = m._23 = m._24 = 0.0f;
	m._31 = m._32 = m._34 = 0.0f;
	m._41 = -(fRight + fLeft) / fWidth;
	m._42 = -(fTop + fBottom) / fHeight;
	return m;
}

inline matrix4 makePerspectiveOffCenterMatrixLH(float fLeft, float fRight, float fBottom, float fTop, float fZNear, float fZFar)
{
	matrix4 m(false);
	float fWidth = fRight - fLeft;
	float fHeight = fTop - fBottom;
	float f2ZN = 2.0f * fZNear;
	m._11 = f2ZN / fWidth;
	m._22 = f2ZN / fHeight;
	m._33 = fZFar / (fZFar - fZNear);
	m._43 = -fZNear * m._33;
	m._34 = 1.0f;
	m._12 = m._13 = m._14 = 0.0f;
	m._21 = m._23 = m._24 = 0.0f;
	m._31 = -(fRight + fLeft) / fWidth;
	m._32 = -(fTop + fBottom) / fHeight;
	m._41 = m._42 = m._44 = 0.0f;
	return m;
}

inline matrix4 makePerspectiveMatrixLH(float fWidth, float fHeight, float fZNear, float fZFar)
{
	matrix4 m(false);
	float f2ZN = 2.0f * fZNear;
	m._11 = f2ZN / fWidth;
	m._22 = f2ZN / fHeight;
	m._33 = fZFar / (fZFar - fZNear);
	m._43 = -fZNear * m._33;
	m._34 = 1.0f;
	return m;
}

inline matrix4 makePerspectiveFovMatrixLH(float fFovY, float fAspect, float fZNear, float fZFar)
{
	matrix4 m(false);
	float fYScale = 1.0f / tanf(fFovY / 2.0f);
	float fXScale = fYScale / fAspect;
	m._11 = fXScale;
	m._22 = fYScale;
	m._33 = fZFar / (fZFar - fZNear);
	m._43 = -fZNear * m._33;
	m._34 = 1.0f;
	return m;
}

}