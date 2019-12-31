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

inline matrix4 rotateX(float vRad)
{
	matrix4 ret;
	ret._33 = ret._22 = (float)cos(vRad);
	ret._32 = (float)sin(vRad);
	ret._23 = (float)-ret._32;

	return ret;
}

inline matrix4 rotateY(float vRad)
{
	matrix4 ret;
	ret._33 = ret._11 = (float)cos(vRad);
	ret._13 = (float)sin(vRad);
	ret._31 = -ret._13;

	return ret;
}

inline matrix4 rotateZ(float vRad)
{
	matrix4 ret;
	ret._22 = ret._11 = (float)cos(vRad);
	ret._21 = (float)sin(vRad);
	ret._12 = -ret._21;

	return ret;
}

inline matrix4 translate(float x, float y, float z)
{
	matrix4 ret(true);
	ret.setTranslation(vector3df(x, y, z));
	return ret;
}

inline matrix4 rotateAxis(const vector3df& vRotAxis, float vRad)
{
	vector3df vecAxis = normalize(vRotAxis);

	matrix4 ret;
	float xx, xy, xz, yy, yz, zz, cosine, sine, one_cs, xsine, ysine, zsine;

	xx = vecAxis.x * vecAxis.x;
	xy = vecAxis.x * vecAxis.y;
	xz = vecAxis.x * vecAxis.z;
	yy = vecAxis.y * vecAxis.y;
	yz = vecAxis.y * vecAxis.z;
	zz = vecAxis.z * vecAxis.z;

	cosine = (float)cos(vRad);
	sine = (float)sin(vRad);
	one_cs = 1.0f - cosine;

	xsine = vecAxis.x * sine;
	ysine = vecAxis.y * sine;
	zsine = vecAxis.z * sine;

	ret._11 = xx + cosine * (1.0f - xx);
	ret._21 = xy * one_cs + zsine;
	ret._31 = xz * one_cs - ysine;
	ret._41 = 0.0f;

	ret._12 = xy * one_cs - zsine;
	ret._22 = yy + cosine * (1.0f - yy);
	ret._32 = yz * one_cs + xsine;
	ret._42 = 0.0f;

	ret._13 = xz * one_cs + ysine;
	ret._23 = yz * one_cs - xsine;
	ret._33 = zz + cosine * (1.0f - zz);
	ret._43 = 0.0f;

	ret._14 = 0.0f;
	ret._24 = 0.0f;
	ret._34 = 0.0f;
	ret._44 = 1.0f;

	return ret;
}

inline matrix4 rotateAxis(const vector3df& vecPos, const vector3df& vecAxis, float vRad)
{
	matrix4 ret = translate(-vecPos.x, -vecPos.y, -vecPos.z);
	ret = ret * rotateAxis(vecAxis, vRad);
	ret = ret * translate(vecPos.x, vecPos.y, vecPos.z);
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
	view._12 = right.y;
	view._13 = right.z;
	view._21 = up.x;
	view._22 = up.y;
	view._23 = up.z;
	view._31 = view_dir.x;
	view._32 = view_dir.y;
	view._33 = view_dir.z;

	view._14 = -dotProduct(right, from);
	view._24 = -dotProduct(up, from);
	view._34 = -dotProduct(view_dir, from);

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
	m._34 = -fZNear * m._33;
	m._44 = 1.0f;
	m._21 = m._31 = m._41 = 0.0f;
	m._12 = m._32 = m._42 = 0.0f;
	m._13 = m._23 = m._43 = 0.0f;
	m._14 = -(fRight + fLeft) / fWidth;
	m._24 = -(fTop + fBottom) / fHeight;
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
	m._34 = -fZNear * m._33;
	m._43 = 1.0f;
	m._21 = m._31 = m._41 = 0.0f;
	m._12 = m._32 = m._42 = 0.0f;
	m._13 = -(fRight + fLeft) / fWidth;
	m._23 = -(fTop + fBottom) / fHeight;
	m._14 = m._24 = m._44 = 0.0f;
	return m;
}

inline matrix4 makePerspectiveMatrixLH(float fWidth, float fHeight, float fZNear, float fZFar)
{
	matrix4 m(false);
	ASSERT(fWidth != 0.f); //divide by zero
	ASSERT(fHeight != 0.f); //divide by zero
	ASSERT(fZNear != fZFar); //divide by zero

	m.M[0] = 2 * fZNear / fWidth;
	m.M[5] = 2 * fZNear / fHeight;
	m.M[10] = fZFar / (fZFar - fZNear);
	m.M[11] = 1;
	m.M[14] = fZNear*fZFar / (fZNear - fZFar);
	return m;
}

inline matrix4 makePerspectiveFovMatrixLH(float fFovY, float fAspect, float fZNear, float fZFar)
{
	matrix4 m(false);
	const float h = reciprocal_((float)tan(fFovY*0.5f));
	ASSERT(fAspect != 0.f); //divide by zero
	const float w = (h / fAspect);

	ASSERT(fZNear != fZFar); //divide by zero
	m.M[0] = w;
	m.M[5] = h;
	m.M[10] = fZFar / (fZFar - fZNear);
	m.M[11] = 1;
	m.M[14] = -fZNear*fZFar / (fZFar - fZNear);
	return m;
}

}