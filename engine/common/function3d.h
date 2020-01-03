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

	mat._00 = vecXAxis.x;
	mat._01 = vecXAxis.y;
	mat._02 = vecXAxis.z;

	mat._10 = vecYAxis.x;
	mat._11 = vecYAxis.y;
	mat._12 = vecYAxis.z;

	mat._20 = vecZAxis.x;
	mat._21 = vecZAxis.y;
	mat._22 = vecZAxis.z;

	mat._30 = vecPos.x;
	mat._31 = vecPos.y;
	mat._32 = vecPos.z;
	mat._33 = 1.0f;

	return mat;
}

inline matrix4 rotateX(float vRad)
{
	matrix4 ret;
	ret._22 = ret._11 = (float)cos(vRad);
	ret._12 = (float)sin(vRad);
	ret._21 = (float)-ret._12;

	return ret;
}

inline matrix4 rotateY(float vRad)
{
	matrix4 ret;
	ret._22 = ret._00 = (float)cos(vRad);
	ret._20 = (float)sin(vRad);
	ret._02 = -ret._20;

	return ret;
}

inline matrix4 rotateZ(float vRad)
{
	matrix4 ret;
	ret._11 = ret._00 = (float)cos(vRad);
	ret._01 = (float)sin(vRad);
	ret._10 = -ret._01;

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
	vOut.x = vIn.x * matView._00 + vIn.y * matView._01 + vIn.z * matView._02;
	vOut.y = vIn.x * matView._10 + vIn.y * matView._11 + vIn.z * matView._12;
	vOut.z = vIn.x * matView._20 + vIn.y * matView._21 + vIn.z * matView._22;
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

	view._00 = right.x;
	view._10 = right.y;
	view._20 = right.z;
	view._01 = up.x;
	view._11 = up.y;
	view._21 = up.z;
	view._02 = view_dir.x;
	view._12 = view_dir.y;
	view._22 = view_dir.z;

	view._30 = -dotProduct(right, from);
	view._31 = -dotProduct(up, from);
	view._32 = -dotProduct(view_dir, from);

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
	m._00 = 2.0f / fWidth;
	m._11 = 2.0f / fHeight;
	m._22 = 1.0f / (fZFar - fZNear);
	m._32 = -fZNear * m._22;
	m._33 = 1.0f;
	m._01 = m._02 = m._03 = 0.0f;
	m._10 = m._12 = m._13 = 0.0f;
	m._20 = m._21 = m._23 = 0.0f;
	m._30 = -(fRight + fLeft) / fWidth;
	m._31 = -(fTop + fBottom) / fHeight;
	return m;
}

inline matrix4 makePerspectiveOffCenterMatrixLH(float fLeft, float fRight, float fBottom, float fTop, float fZNear, float fZFar)
{
	matrix4 m(false);
	float fWidth = fRight - fLeft;
	float fHeight = fTop - fBottom;
	float f2ZN = 2.0f * fZNear;
	m._00 = f2ZN / fWidth;
	m._11 = f2ZN / fHeight;
	m._22 = fZFar / (fZFar - fZNear);
	m._32 = -fZNear * m._22;
	m._23 = 1.0f;
	m._01 = m._02 = m._03 = 0.0f;
	m._10 = m._12 = m._13 = 0.0f;
	m._20 = -(fRight + fLeft) / fWidth;
	m._21 = -(fTop + fBottom) / fHeight;
	m._30 = m._31 = m._33 = 0.0f;
	return m;
}

inline matrix4 makePerspectiveMatrixLH(float fWidth, float fHeight, float fZNear, float fZFar)
{
	matrix4 m(false);
	float f2ZN = 2.0f * fZNear;
	m._00 = f2ZN / fWidth;
	m._11 = f2ZN / fHeight;
	m._22 = fZFar / (fZFar - fZNear);
	m._32 = -fZNear * m._22;
	m._23 = 1.0f;
	return m;
}

inline matrix4 makePerspectiveFovMatrixLH(float fFovY, float fAspect, float fZNear, float fZFar)
{
	matrix4 m(false);
	float fYScale = 1.0f / tanf(fFovY / 2.0f);
	float fXScale = fYScale / fAspect;
	m._00 = fXScale;
	m._11 = fYScale;
	m._22 = fZFar / (fZFar - fZNear);
	m._32 = -fZNear * m._22;
	m._23 = 1.0f;
	return m;
}

}