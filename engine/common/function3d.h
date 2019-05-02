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
	matrix4   mat;
	vector3df   vecXAxis, vecYAxis, vecZAxis;

	vecZAxis = normalize(vecDir);
	vecYAxis = normalize(vecUp);
	vecXAxis = normalize(crossProduct(vecYAxis, vecZAxis));

	assert(vecXAxis.magnitude() > 1e-3);

	memset(&mat, 0, sizeof(mat));
	mat.M[0][0] = vecXAxis.x;
	mat.M[0][1] = vecXAxis.y;
	mat.M[0][2] = vecXAxis.z;

	mat.M[1][0] = vecYAxis.x;
	mat.M[1][1] = vecYAxis.y;
	mat.M[1][2] = vecYAxis.z;

	mat.M[2][0] = vecZAxis.x;
	mat.M[2][1] = vecZAxis.y;
	mat.M[2][2] = vecZAxis.z;

	mat.M[3][0] = vecPos.x;
	mat.M[3][1] = vecPos.y;
	mat.M[3][2] = vecPos.z;
	mat.M[3][3] = 1.0f;

	return mat;
}

}