#pragma once

#include "vector2d.h"
#include "vector3d.h"
#include "vector4d.h"
#include "plane3d.h"
#include "aabbox3d.h"

//row major matrix
template <class T>
class CMatrix4
{
public:
	//
	CMatrix4() { makeIdentity(); }
	explicit CMatrix4(bool Identity) { if (Identity) makeIdentity(); else makeZero(); }
	CMatrix4(const CMatrix4& other){ (*this) = other; }

	const T* pointer() const { return M; }
	T* pointer() { return M; }

	CMatrix4<T>& operator=(const CMatrix4<T> &other);

	bool operator==(const CMatrix4<T> &other) const;
	bool operator!=(const CMatrix4<T> &other) const { return !(*this == other); }

	CMatrix4<T> operator+(const CMatrix4<T>& other) const;
	CMatrix4<T>& operator+=(const CMatrix4<T>& other);
	CMatrix4<T> operator-(const CMatrix4<T>& other) const;
	CMatrix4<T>& operator-=(const CMatrix4<T>& other);

	CMatrix4<T> operator*(const CMatrix4<T>& other) const;
	CMatrix4<T>& operator*=(const CMatrix4<T>& other) { return setbyproduct(*this, other); }
	CMatrix4<T> operator*(const T& scalar) const;
	CMatrix4<T>& operator*=(const T& scalar);
	CMatrix4<T>& setbyproduct(const CMatrix4<T>& other_a, const CMatrix4<T>& other_b);

	//
	CMatrix4<T>& makeIdentity()
	{
		M[1] = M[2] = M[3] = M[4] =
			M[6] = M[7] = M[8] = M[9] =
			M[11] = M[12] = M[13] = M[14] = 0;
		M[0] = M[5] = M[10] = M[15] = (T)1;
		return *this;
	}
	CMatrix4<T>& makeZero(){ memset(M, 0, 16 * sizeof(T)); return *this; }
	CMatrix4<T> getInverse() const;
	float getDeterminant() const;
	void transpose();
	bool equals(const CMatrix4<T>& other, const T tolerance = (T)ROUNDING_ERROR_f32) const;

	bool isIdentity() const;
	bool isZero() const;
	//
	vector3df transformVector(const vector3df& vect, float& z) const;
	vector3df transformVector(const vector3df& vect) const;
	plane3df transformPlane(const plane3df& plane) const;
	aabbox3df transformBox(const aabbox3df& box) const;
	vector3df rotateVector(const vector3df& vect) const;
	vector3df inverseRotateVector(const vector3df& vect) const;

	vector4df getRow(int i) const
	{
		switch (i)
		{
		case 0: return vector4df(_11, _12, _13, _14);
		case 1: return vector4df(_21, _22, _23, _24);
		case 2: return vector4df(_31, _32, _33, _34);
		case 3: return vector4df(_41, _42, _43, _44);
		default: ASSERT(false); return vector4df::Zero();
		}
	}
	vector4df getCol(int i) const
	{
		switch (i)
		{
		case 0: return vector4df(_11, _21, _31, _41);
		case 1: return vector4df(_12, _22, _32, _42);
		case 2: return vector4df(_13, _23, _33, _43);
		case 3: return vector4df(_14, _24, _34, _44);
		default: ASSERT(false); return vector4df::Zero();
		}
	}

	//	Set row and column
	void setRow(int i, const vector4df& v) 
	{ 
		switch (i)
		{
		case 0:
			_11 = v.x; _12 = v.y; _13 = v.z; _14 = v.w;
			break;
		case 1: 
			_21 = v.x; _22 = v.y; _23 = v.z; _24 = v.w;
			break;
		case 2: 
			_31 = v.x; _32 = v.y; _33 = v.z; _34 = v.w;
			break;
		case 3: 
			_41 = v.x; _42 = v.y; _43 = v.z; _44 = v.w;
			break;
		default: 
			ASSERT(false);
		}
	}

	void setCol(int i, const vector4df& v) 
	{ 
		switch (i)
		{
		case 0:
			_11 = v.x; _21 = v.y; _31 = v.z; _41 = v.w;
			break;
		case 1:
			_12 = v.x; _22 = v.y; _32 = v.z; _42 = v.w;
			break;
		case 2:
			_13 = v.x; _23 = v.y; _33 = v.z; _43 = v.w;
			break;
		case 3:
			_14 = v.x; _24 = v.y; _34 = v.z; _44 = v.w;
			break;
		default:
			ASSERT(false);
		}
	}

	CMatrix4<T>& setTranslation(const vector3d<T>& translation);
	vector3d<T> getTranslation() const { return vector3d<T>(M[12], M[13], M[14]); }

	CMatrix4<T>& scale(const vector3d<T>& scale);
	CMatrix4<T>& scale(const T scale) { return scale(vector3d<T>(scale, scale, scale)); }
	CMatrix4<T>& setScale(const vector3d<T>& scale);
	vector3d<T> getScale() const;

	vector3d<T> getDir() const { return f3d::normalize(getRow(2)); }
	vector3d<T> getUp() const { return f3d::normalize(getRow(1)); }
	vector3d<T> getRight() const { return f3d::normalize(getRow(0)); }


	// camera view, perspective
	//camera
	CMatrix4<T>& buildCameraLookAtMatrixLH(
		const vector3df& position,
		const vector3df& target,
		const vector3df& upVector);

	CMatrix4<T>& buildCameraLookAtMatrixRH(
		const vector3df& position,
		const vector3df& target,
		const vector3df& upVector);

	//projection
	CMatrix4<T>& buildProjectionMatrixPerspectiveFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixPerspectiveFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixPerspectiveRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixPerspectiveLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixOrthoFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixOrthoFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixOrthoLH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	CMatrix4<T>& buildProjectionMatrixOrthoRH(float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar);

	CMatrix4<T>& setM(const T* data);

	static const CMatrix4<T>& Identity() { static CMatrix4<T> m(true); return m; }
	static const CMatrix4<T>& Zero() { static CMatrix4<T> m(false); return m; }

	CMatrix4<T>& buildMatrix(const vector3d<T>& vDir, const vector3d<T>& vUp, const vector3d<T>& vPos);

public:
	union
	{
		struct
		{
			T M[16];
		};
		struct
		{
			T _11, _12, _13, _14;
			T _21, _22, _23, _24;
			T _31, _32, _33, _34;
			T _41, _42, _43, _44;
		};
	};

	private:
		float Det3x3(float a11, float a12, float a13, float a21, float a22, float a23,
			float a31, float a32, float a33) const
		{
			return a11 * a22 * a33 + a21 * a32 * a13 + a31 * a12 * a23 -
				a13 * a22 * a31 - a23 * a32 * a11 - a33 * a12 * a21;
		}
};

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildMatrix(const vector3d<T>& vDir, const vector3d<T>& vUp, const vector3d<T>& vPos)
{
	vector3d<T>	vX, vY, vZ;

	vZ = vDir;
	vZ.normalize();
	vY = vUp;
	vY.normalize();
	vX = vY.crossProduct(vZ);
	vX.normalize();

	_11 = vX.x;
	_12 = vX.y;
	_13 = vX.z;
	_14 = (T)0.0f;

	_21 = vY.x;
	_22 = vY.y;
	_23 = vY.z;
	_24 = (T)0.0f;

	_31 = vZ.x;
	_32 = vZ.y;
	_33 = vZ.z;
	_34 = (T)0.0f;

	_41 = vPos.x;
	_42 = vPos.y;
	_43 = vPos.z;
	_44 = (T)1.0f;

	return *this;
}

template <class T>
inline void CMatrix4<T>::transpose()
{
	T tmp;
	tmp = _12;
	_12 = _21;
	_21 = tmp;

	tmp = _13;
	_13 = _31;
	_31 = tmp;

	tmp = _14;
	_14 = _41;
	_41 = tmp;

	tmp = _23;
	_23 = _32;
	_32 = tmp;

	tmp = _24;
	_24 = _42;
	_42 = tmp;

	tmp = _34;
	_34 = _43;
	_43 = tmp;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setM(const T* data)
{
	memcpy(M, data, 16 * sizeof(T));
	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::operator=(const CMatrix4<T> &other)
{
	ASSERT(this != &other);
	memcpy(M, other.M, 16 * sizeof(T));
	return *this;
}

template <class T>
inline bool CMatrix4<T>::operator==(const CMatrix4<T>& other) const
{
	if (this == &other) return true;
	return memcmp(this, &other, sizeof(CMatrix4<T>)) == 0;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::operator+(const CMatrix4<T>& other) const
{
	CMatrix4<T> temp;

	temp[0] = M[0] + other[0];
	temp[1] = M[1] + other[1];
	temp[2] = M[2] + other[2];
	temp[3] = M[3] + other[3];
	temp[4] = M[4] + other[4];
	temp[5] = M[5] + other[5];
	temp[6] = M[6] + other[6];
	temp[7] = M[7] + other[7];
	temp[8] = M[8] + other[8];
	temp[9] = M[9] + other[9];
	temp[10] = M[10] + other[10];
	temp[11] = M[11] + other[11];
	temp[12] = M[12] + other[12];
	temp[13] = M[13] + other[13];
	temp[14] = M[14] + other[14];
	temp[15] = M[15] + other[15];

	return temp;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::operator+=(const CMatrix4<T>& other)
{
	M[0] += other[0];
	M[1] += other[1];
	M[2] += other[2];
	M[3] += other[3];
	M[4] += other[4];
	M[5] += other[5];
	M[6] += other[6];
	M[7] += other[7];
	M[8] += other[8];
	M[9] += other[9];
	M[10] += other[10];
	M[11] += other[11];
	M[12] += other[12];
	M[13] += other[13];
	M[14] += other[14];
	M[15] += other[15];

	return *this;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::operator-(const CMatrix4<T>& other) const
{
	CMatrix4<T> temp;

	temp[0] = M[0] - other[0];
	temp[1] = M[1] - other[1];
	temp[2] = M[2] - other[2];
	temp[3] = M[3] - other[3];
	temp[4] = M[4] - other[4];
	temp[5] = M[5] - other[5];
	temp[6] = M[6] - other[6];
	temp[7] = M[7] - other[7];
	temp[8] = M[8] - other[8];
	temp[9] = M[9] - other[9];
	temp[10] = M[10] - other[10];
	temp[11] = M[11] - other[11];
	temp[12] = M[12] - other[12];
	temp[13] = M[13] - other[13];
	temp[14] = M[14] - other[14];
	temp[15] = M[15] - other[15];

	return temp;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::operator-=(const CMatrix4<T>& other)
{
	M[0] -= other[0];
	M[1] -= other[1];
	M[2] -= other[2];
	M[3] -= other[3];
	M[4] -= other[4];
	M[5] -= other[5];
	M[6] -= other[6];
	M[7] -= other[7];
	M[8] -= other[8];
	M[9] -= other[9];
	M[10] -= other[10];
	M[11] -= other[11];
	M[12] -= other[12];
	M[13] -= other[13];
	M[14] -= other[14];
	M[15] -= other[15];

	return *this;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::operator*(const CMatrix4<T>& other) const
{
	CMatrix4<T> res;
	const CMatrix4<T>& lhs = *this;
	const CMatrix4<T>& rhs = other;

	res._11 = lhs._11 * rhs._11 + lhs._12 * rhs._21 + lhs._13 * rhs._31 + lhs._14 * rhs._41;
	res._12 = lhs._11 * rhs._12 + lhs._12 * rhs._22 + lhs._13 * rhs._32 + lhs._14 * rhs._42;
	res._13 = lhs._11 * rhs._13 + lhs._12 * rhs._23 + lhs._13 * rhs._33 + lhs._14 * rhs._43;
	res._14 = lhs._11 * rhs._14 + lhs._12 * rhs._24 + lhs._13 * rhs._34 + lhs._14 * rhs._44;

	res._21 = lhs._21 * rhs._11 + lhs._22 * rhs._21 + lhs._23 * rhs._31 + lhs._24 * rhs._41;
	res._22 = lhs._21 * rhs._12 + lhs._22 * rhs._22 + lhs._23 * rhs._32 + lhs._24 * rhs._42;
	res._23 = lhs._21 * rhs._13 + lhs._22 * rhs._23 + lhs._23 * rhs._33 + lhs._24 * rhs._43;
	res._24 = lhs._21 * rhs._14 + lhs._22 * rhs._24 + lhs._23 * rhs._34 + lhs._24 * rhs._44;

	res._31 = lhs._31 * rhs._11 + lhs._32 * rhs._21 + lhs._33 * rhs._31 + lhs._34 * rhs._41;
	res._32 = lhs._31 * rhs._12 + lhs._32 * rhs._22 + lhs._33 * rhs._32 + lhs._34 * rhs._42;
	res._33 = lhs._31 * rhs._13 + lhs._32 * rhs._23 + lhs._33 * rhs._33 + lhs._34 * rhs._43;
	res._34 = lhs._31 * rhs._14 + lhs._32 * rhs._24 + lhs._33 * rhs._34 + lhs._34 * rhs._44;

	res._41 = lhs._41 * rhs._11 + lhs._42 * rhs._21 + lhs._43 * rhs._31 + lhs._44 * rhs._41;
	res._42 = lhs._41 * rhs._12 + lhs._42 * rhs._22 + lhs._43 * rhs._32 + lhs._44 * rhs._42;
	res._43 = lhs._41 * rhs._13 + lhs._42 * rhs._23 + lhs._43 * rhs._33 + lhs._44 * rhs._43;
	res._44 = lhs._41 * rhs._14 + lhs._42 * rhs._24 + lhs._43 * rhs._34 + lhs._44 * rhs._44;

	return res;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::operator*(const T& scalar) const
{
	CMatrix4<T> temp;

	temp[0] = M[0] * scalar;
	temp[1] = M[1] * scalar;
	temp[2] = M[2] * scalar;
	temp[3] = M[3] * scalar;
	temp[4] = M[4] * scalar;
	temp[5] = M[5] * scalar;
	temp[6] = M[6] * scalar;
	temp[7] = M[7] * scalar;
	temp[8] = M[8] * scalar;
	temp[9] = M[9] * scalar;
	temp[10] = M[10] * scalar;
	temp[11] = M[11] * scalar;
	temp[12] = M[12] * scalar;
	temp[13] = M[13] * scalar;
	temp[14] = M[14] * scalar;
	temp[15] = M[15] * scalar;

	return temp;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::operator*=(const T& scalar)
{
	M[0] *= scalar;
	M[1] *= scalar;
	M[2] *= scalar;
	M[3] *= scalar;
	M[4] *= scalar;
	M[5] *= scalar;
	M[6] *= scalar;
	M[7] *= scalar;
	M[8] *= scalar;
	M[9] *= scalar;
	M[10] *= scalar;
	M[11] *= scalar;
	M[12] *= scalar;
	M[13] *= scalar;
	M[14] *= scalar;
	M[15] *= scalar;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setbyproduct(const CMatrix4<T>& other_a, const CMatrix4<T>& other_b)
{
	const CMatrix4<T>& lhs = other_a;
	const CMatrix4<T>& rhs = other_b;

	_11 = lhs._11 * rhs._11 + lhs._12 * rhs._21 + lhs._13 * rhs._31 + lhs._14 * rhs._41;
	_12 = lhs._11 * rhs._12 + lhs._12 * rhs._22 + lhs._13 * rhs._32 + lhs._14 * rhs._42;
	_13 = lhs._11 * rhs._13 + lhs._12 * rhs._23 + lhs._13 * rhs._33 + lhs._14 * rhs._43;
	_14 = lhs._11 * rhs._14 + lhs._12 * rhs._24 + lhs._13 * rhs._34 + lhs._14 * rhs._44;

	_21 = lhs._21 * rhs._11 + lhs._22 * rhs._21 + lhs._23 * rhs._31 + lhs._24 * rhs._41;
	_22 = lhs._21 * rhs._12 + lhs._22 * rhs._22 + lhs._23 * rhs._32 + lhs._24 * rhs._42;
	_23 = lhs._21 * rhs._13 + lhs._22 * rhs._23 + lhs._23 * rhs._33 + lhs._24 * rhs._43;
	_24 = lhs._21 * rhs._14 + lhs._22 * rhs._24 + lhs._23 * rhs._34 + lhs._24 * rhs._44;

	_31 = lhs._31 * rhs._11 + lhs._32 * rhs._21 + lhs._33 * rhs._31 + lhs._34 * rhs._41;
	_32 = lhs._31 * rhs._12 + lhs._32 * rhs._22 + lhs._33 * rhs._32 + lhs._34 * rhs._42;
	_33 = lhs._31 * rhs._13 + lhs._32 * rhs._23 + lhs._33 * rhs._33 + lhs._34 * rhs._43;
	_34 = lhs._31 * rhs._14 + lhs._32 * rhs._24 + lhs._33 * rhs._34 + lhs._34 * rhs._44;

	_41 = lhs._41 * rhs._11 + lhs._42 * rhs._21 + lhs._43 * rhs._31 + lhs._44 * rhs._41;
	_42 = lhs._41 * rhs._12 + lhs._42 * rhs._22 + lhs._43 * rhs._32 + lhs._44 * rhs._42;
	_43 = lhs._41 * rhs._13 + lhs._42 * rhs._23 + lhs._43 * rhs._33 + lhs._44 * rhs._43;
	_44 = lhs._41 * rhs._14 + lhs._42 * rhs._24 + lhs._43 * rhs._34 + lhs._44 * rhs._44;

	return *this;
}

template <class T>
inline float CMatrix4<T>::getDeterminant() const
{
	float fDet;
	fDet = _11 * _22 * _33 * _44;
	fDet += _21 * _32 * _43 * _14;
	fDet += _31 * _42 * _13 * _24;
	fDet += _41 * _12 * _23 * _34;
	fDet -= _41 * _32 * _23 * _14;
	fDet -= _42 * _33 * _24 * _11;
	fDet -= _43 * _34 * _21 * _12;
	fDet -= _44 * _31 * _22 * _13;

	return fDet;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::getInverse() const
{
	CMatrix4<T> out;

	float d = getDeterminant();
	d = reciprocal_(d);

	out._11 = Det3x3(_22, _23, _24, _32, _33, _34, _42, _43, _44) * d;
	out._12 = -Det3x3(_12, _13, _14, _32, _33, _34, _42, _43, _44) * d;
	out._13 = Det3x3(_12, _13, _14, _22, _23, _24, _42, _43, _44) * d;
	out._14 = -Det3x3(_12, _13, _14, _22, _23, _24, _32, _33, _34) * d;

	out._21 = -Det3x3(_21, _23, _24, _31, _33, _34, _41, _43, _44) * d;
	out._22 = Det3x3(_11, _13, _14, _31, _33, _34, _41, _43, _44) * d;
	out._23 = -Det3x3(_11, _13, _14, _21, _23, _24, _41, _43, _44) * d;
	out._24 = Det3x3(_11, _13, _14, _21, _23, _24, _31, _33, _34) * d;

	out._31 = Det3x3(_21, _22, _24, _31, _32, _34, _41, _42, _44) * d;
	out._32 = -Det3x3(_11, _12, _14, _31, _32, _34, _41, _42, _44) * d;
	out._33 = Det3x3(_11, _12, _14, _21, _22, _24, _41, _42, _44) * d;
	out._34 = -Det3x3(_11, _12, _14, _21, _22, _24, _31, _32, _34) * d;

	out._41 = -Det3x3(_21, _22, _23, _31, _32, _33, _41, _42, _43) * d;
	out._42 = Det3x3(_11, _12, _13, _31, _32, _33, _41, _42, _43) * d;
	out._43 = -Det3x3(_11, _12, _13, _21, _22, _23, _41, _42, _43) * d;
	out._44 = Det3x3(_11, _12, _13, _21, _22, _23, _31, _32, _33) * d;

	return out;
}

template <class T>
inline bool CMatrix4<T>::equals(const CMatrix4<T>& other, const T tolerance) const
{
	for (int32_t i = 0; i < 16; ++i)
	if (!equals_(M[i], other.M[i], tolerance))
		return false;

	return true;
}

template <class T>
inline bool CMatrix4<T>::isIdentity() const
{
	return (*this) == Identity();
}

template <class T>
bool CMatrix4<T>::isZero() const
{
	return (*this) == Zero();
}

template <class T>
inline vector3df CMatrix4<T>::transformVector(const vector3df& vect, float& z) const
{
	vector3df vector;

	vector.x = vect.x * _11 + vect.y * _21 + vect.z * _31 + _41;
	vector.y = vect.x * _12 + vect.y * _22 + vect.z * _32 + _42;
	vector.z = vect.x * _13 + vect.y * _23 + vect.z * _33 + _43;
	z = vect.x * _14 + vect.y * _24 + vect.z * _34 + _44;

	return vector;
}

template <class T>
inline vector3df CMatrix4<T>::transformVector(const vector3df& vect) const
{
	vector3df vector;

	vector.x = vect.x * _11 + vect.y * _21 + vect.z * _31 + _41;
	vector.y = vect.x * _12 + vect.y * _22 + vect.z * _32 + _42;
	vector.z = vect.x * _13 + vect.y * _23 + vect.z * _33 + _43;

	return vector;
}

template <class T>
inline plane3df CMatrix4<T>::transformPlane(const plane3df& plane) const
{
	vector3df point = transformVector(plane.getMemberPoint());

	CMatrix4<T> m = getInverse();
	m.transpose();

	vector3df normal = m.transformVector(plane.Normal);

	return plane3df(point, normal);
}

template <class T>
inline aabbox3df CMatrix4<T>::transformBox(const aabbox3df& box) const
{
	vector3df vCenter = transformVector(box.getCenter());

	const vector3df& ext = box.getExtent();
	vector3df vAxisX = rotateVector(vector3df(ext.x, 0, 0));
	vector3df vAxisY = rotateVector(vector3df(0, ext.y, 0));
	vector3df vAxisZ = rotateVector(vector3df(0, 0, ext.z));

	vector3df vExtends;
	vExtends.x = fabs(vAxisX.x) + fabs(vAxisY.x) + fabs(vAxisZ.x);
	vExtends.y = fabs(vAxisX.y) + fabs(vAxisY.y) + fabs(vAxisZ.y);
	vExtends.z = fabs(vAxisX.z) + fabs(vAxisY.z) + fabs(vAxisZ.z);

	aabbox3df aabbNew;
	aabbNew.setByCenterExtent(vCenter, vExtends);

	return aabbNew;
}

template <class T>
inline vector3df CMatrix4<T>::rotateVector(const vector3df& vect) const
{
	vector3df tmp;
	tmp.x = vect.x * _11 + vect.y * _21 + vect.z * _31;
	tmp.y = vect.x * _12 + vect.y * _22 + vect.z * _32;
	tmp.z = vect.x * _13 + vect.y * _23 + vect.z * _33;
	return tmp;
}

template <class T>
inline  vector3df CMatrix4<T>::inverseRotateVector(const vector3df& vect) const
{
	vector3df tmp;
	tmp.x = vect.x * _11 + vect.y * _12 + vect.z * _13;
	tmp.y = vect.x * _21 + vect.y * _22 + vect.z * _23;
	tmp.z = vect.x * _31 + vect.y * _32 + vect.z * _33;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setTranslation(const vector3d<T>& translation)
{
	_41 = translation.x;
	_42 = translation.y;
	_43 = translation.z;
	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::scale(const vector3d<T>& scale)
{
	ASSERT(scale.x > 0);			// 0
	ASSERT(scale.y > 0);			// 5
	ASSERT(scale.z > 0);			// 10

	_11 *= scale.x;
	_21 *= scale.x;
	_31 *= scale.x;
	_41 *= scale.x;

	_12 *= scale.y;
	_22 *= scale.y;
	_32 *= scale.y;
	_42 *= scale.y;

	_13 *= scale.z;
	_23 *= scale.z;
	_33 *= scale.z;
	_43 *= scale.z;

	return (*this);
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setScale(const vector3d<T>& scale)
{
	_11 = scale.x;	_12 = 0; _13 = 0; _14 = 0;
	_11 = 0;	_12 = scale.y; _13 = 0; _14 = 0;
	_11 = 0;	_12 = 0; _13 = scale.z; _14 = 0;
	_41 = 0;	_42 = 0; _43 = 0; _44 = 1;

	return (*this);
}

template <class T>
inline vector3d<T> CMatrix4<T>::getScale() const
{
	// We have to do the full calculation.
	return vector3d<T>(squareroot_(M[0] * M[0] + M[1] * M[1] + M[2] * M[2]),
		squareroot_(M[4] * M[4] + M[5] * M[5] + M[6] * M[6]),
		squareroot_(M[8] * M[8] + M[9] * M[9] + M[10] * M[10]));
}

template <class T>
inline  CMatrix4<T>& CMatrix4<T>::buildCameraLookAtMatrixLH(
	const vector3df& position,
	const vector3df& target,
	const vector3df& upVector)
{
	vector3df zaxis = target - position;
	zaxis.normalize();

	vector3df xaxis = upVector.crossProduct(zaxis);
	xaxis.normalize();

	vector3df yaxis = zaxis.crossProduct(xaxis);

	_11 = (T)xaxis.x;
	_12 = (T)yaxis.x;
	_13 = (T)zaxis.x;
	_14 = 0;

	_21 = (T)xaxis.y;
	_22 = (T)yaxis.y;
	_23 = (T)zaxis.y;
	_24 = 0;

	_31 = (T)xaxis.z;
	_32 = (T)yaxis.z;
	_33 = (T)zaxis.z;
	_34 = 0;

	_41 = (T)-xaxis.dotProduct(position);
	_42 = (T)-yaxis.dotProduct(position);
	_43 = (T)-zaxis.dotProduct(position);
	_44 = 1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildCameraLookAtMatrixRH(
	const vector3df& position,
	const vector3df& target,
	const vector3df& upVector)
{
	vector3df zaxis = position - target;
	zaxis.normalize();

	vector3df xaxis = upVector.crossProduct(zaxis);
	xaxis.normalize();

	vector3df yaxis = zaxis.crossProduct(xaxis);

	_11 = (T)xaxis.x;
	_12 = (T)yaxis.x;
	_13 = (T)zaxis.x;
	_14 = 0;

	_21 = (T)xaxis.y;
	_22 = (T)yaxis.y;
	_23 = (T)zaxis.y;
	_24 = 0;

	_31 = (T)xaxis.z;
	_32 = (T)yaxis.z;
	_33 = (T)zaxis.z;
	_34 = 0;

	_41 = (T)-xaxis.dotProduct(position);
	_42 = (T)-yaxis.dotProduct(position);
	_43 = (T)-zaxis.dotProduct(position);
	_44 = 1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveFovLH(
	float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const float h = reciprocal_((float)tan(fieldOfViewRadians*0.5f));
	ASSERT(aspectRatio != 0.f); //divide by zero
	const float w = (h / aspectRatio);

	ASSERT(zNear != zFar); //divide by zero
	_11 = w;
	_12 = 0;
	_13 = 0;
	_14 = 0;

	_21 = 0;
	_22 = (T)h;
	_23 = 0;
	_24 = 0;

	_31 = 0;
	_32 = 0;
	_33 = (T)(zFar / (zFar - zNear));
	_34 = 1;

	_41 = 0;
	_42 = 0;
	_43 = (T)(-zNear*zFar / (zFar - zNear));
	_44 = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveFovRH(
	float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const float h = reciprocal_(tan(fieldOfViewRadians*0.5f));
	ASSERT(aspectRatio != 0.f); //divide by zero
	const T w = h / aspectRatio;

	ASSERT(zNear != zFar); //divide by zero
	_11 = w;
	_12 = 0;
	_13 = 0;
	_14 = 0;

	_21 = 0;
	_22 = (T)h;
	_23 = 0;
	_24 = 0;

	_31 = 0;
	_32 = 0;
	_33 = (T)(zFar / (zNear - zFar)); // DirectX version
	//		M[10] = (T)(zFar+zNear/(zNear-zFar)); // OpenGL version
	_34 = -1;

	_41 = 0;
	_42 = 0;
	_43 = (T)(zNear*zFar / (zNear - zFar)); // DirectX version
	//		M[14] = (T)(2.0f*zNear*zFar/(zNear-zFar)); // OpenGL version
	_44 = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveLH(
	float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	ASSERT(widthOfViewVolume != 0.f); //divide by zero
	ASSERT(heightOfViewVolume != 0.f); //divide by zero
	ASSERT(zNear != zFar); //divide by zero
	_11 = (T)(2 * zNear / widthOfViewVolume);
	_12 = 0;
	_13 = 0;
	_14 = 0;

	_21 = 0;
	_22 = (T)(2 * zNear / heightOfViewVolume);
	_23 = 0;
	_24 = 0;

	_31 = 0;
	_32 = 0;
	_33 = (T)(zFar / (zFar - zNear));
	_34 = 1;

	_41 = 0;
	_42 = 0;
	_43 = (T)(zNear*zFar / (zNear - zFar));
	_44 = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixPerspectiveRH(
	float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	ASSERT(widthOfViewVolume != 0.f); //divide by zero
	ASSERT(heightOfViewVolume != 0.f); //divide by zero
	ASSERT(zNear != zFar); //divide by zero
	_11 = (T)(2 * zNear / widthOfViewVolume);
	_12 = 0;
	_13 = 0;
	_14 = 0;

	_21 = 0;
	_22 = (T)(2 * zNear / heightOfViewVolume);
	_23 = 0;
	_24 = 0;

	_31 = 0;
	_32 = 0;
	_33 = (T)(zFar / (zNear - zFar));
	_34 = -1;

	_41 = 0;
	_42 = 0;
	_43 = (T)(zNear*zFar / (zNear - zFar));
	_44 = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const float h = reciprocal_((float)tan(fieldOfViewRadians*0.5));
	ASSERT(aspectRatio != 0.f); //divide by zero
	const T w = (T)(h / aspectRatio);

	ASSERT(zNear != zFar); //divide by zero
	ASSERT(zNear != 0.0f);  //divide by zero

	_11 = w / zNear;
	_12 = 0;
	_13 = 0;
	_14 = 0;

	_21 = 0;
	_22 = (T)h / zNear;
	_23 = 0;
	_24 = 0;

	_31 = 0;
	_32 = 0;
	_33 = (T)(zFar / (zFar - zNear));
	_34 = 1;

	_41 = 0;
	_42 = 0;
	_43 = (T)(-zNear*zFar / (zFar - zNear));
	_44 = 0;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoFovRH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
	const float h = reciprocal_((float)tan(fieldOfViewRadians*0.5));
	ASSERT(aspectRatio != 0.f); //divide by zero
	const T w = (T)(h / aspectRatio);

	ASSERT(zNear != zFar); //divide by zero
	ASSERT(zNear != 0.0f);  //divide by zero

	_11 = w / zNear;
	_12 = 0;
	_13 = 0;
	_14 = 0;

	_21 = 0;
	_22 = (T)h / zNear;
	_23 = 0;
	_24 = 0;

	_31 = 0;
	_32 = 0;
	_33 = (T)(1 / (zNear - zFar));
	_34 = 0;

	_41 = 0;
	_42 = 0;
	_43 = (T)(zNear / (zNear - zFar));
	_44 = -1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoLH(
	float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	ASSERT(widthOfViewVolume != 0.f); //divide by zero
	ASSERT(heightOfViewVolume != 0.f); //divide by zero
	ASSERT(zNear != zFar); //divide by zero
	_11 = (T)(2 / widthOfViewVolume);
	_12 = 0;
	_13 = 0;
	_14 = 0;

	_21 = 0;
	_22 = (T)(2 / heightOfViewVolume);
	_23 = 0;
	_24 = 0;

	_31 = 0;
	_32 = 0;
	_33 = (T)(1 / (zFar - zNear));
	_34 = 0;

	_41 = 0;
	_42 = 0;
	_43 = (T)(zNear / (zNear - zFar));
	_44 = 1;

	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::buildProjectionMatrixOrthoRH(
	float widthOfViewVolume, float heightOfViewVolume, float zNear, float zFar)
{
	ASSERT(widthOfViewVolume != 0.f); //divide by zero
	ASSERT(heightOfViewVolume != 0.f); //divide by zero
	ASSERT(zNear != zFar); //divide by zero
	_11 = (T)(2 / widthOfViewVolume);
	_12 = 0;
	_13 = 0;
	_14 = 0;

	_21 = 0;
	_22 = (T)(2 / heightOfViewVolume);
	_23 = 0;
	_24 = 0;

	_31 = 0;
	_32 = 0;
	_33 = (T)(1 / (zNear - zFar));
	_34 = 0;

	_41 = 0;
	_42 = 0;
	_43 = (T)(zNear / (zNear - zFar));
	_44 = -1;

	return *this;
}

typedef CMatrix4<float> matrix4;
