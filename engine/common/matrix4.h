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
		_00 = (T)1.0f;
		_01 = (T)0.0f;
		_02 = (T)0.0f;
		_03 = (T)0.0f;

		_10 = (T)0.0f;
		_11 = (T)1.0f;
		_12 = (T)0.0f;
		_13 = (T)0.0f;

		_20 = (T)0.0f;
		_21 = (T)0.0f;
		_22 = (T)1.0f;
		_23 = (T)0.0f;

		_30 = (T)0.0f;
		_31 = (T)0.0f;
		_32 = (T)0.0f;
		_33 = (T)1.0f;
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
	vector3df multiplyPoint(const vector3df& vect, float& z) const;
	vector3df multiplyPoint(const vector3df& vect) const;
	vector3df multiplyVector(const vector3df& vect) const;

	plane3df transformPlane(const plane3df& plane) const;
	aabbox3df transformBox(const aabbox3df& box) const;
	vector3df rotateVector(const vector3df& vect) const;
	vector3df inverseRotateVector(const vector3df& vect) const;

	vector4df getRow(int i) const
	{
		switch (i)
		{
		case 0: return vector4df(_00, _01, _02, _03);
		case 1: return vector4df(_10, _11, _12, _13);
		case 2: return vector4df(_20, _21, _22, _23);
		case 3: return vector4df(_30, _31, _32, _33);
		default: ASSERT(false); return vector4df::Zero();
		}
	}
	vector4df getCol(int i) const
	{
		switch (i)
		{
		case 0: return vector4df(_00, _10, _20, _30);
		case 1: return vector4df(_01, _11, _21, _31);
		case 2: return vector4df(_02, _12, _22, _32);
		case 3: return vector4df(_03, _13, _23, _33);
		default: ASSERT(false); return vector4df::Zero();
		}
	}

	//	Set row and column
	void setRow(int i, const vector4df& v) 
	{ 
		switch (i)
		{
		case 0:
			_00 = v.x; _01 = v.y; _02 = v.z; _03 = v.w;
			break;
		case 1: 
			_10 = v.x; _11 = v.y; _12 = v.z; _13 = v.w;
			break;
		case 2: 
			_20 = v.x; _21 = v.y; _22 = v.z; _23 = v.w;
			break;
		case 3: 
			_30 = v.x; _31 = v.y; _32 = v.z; _33 = v.w;
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
			_00 = v.x; _10 = v.y; _20 = v.z; _30 = v.w;
			break;
		case 1:
			_01 = v.x; _11 = v.y; _21 = v.z; _31 = v.w;
			break;
		case 2:
			_02 = v.x; _12 = v.y; _22 = v.z; _32 = v.w;
			break;
		case 3:
			_03 = v.x; _13 = v.y; _23 = v.z; _33 = v.w;
			break;
		default:
			ASSERT(false);
		}
	}

	CMatrix4<T>& setTranslation(const vector3d<T>& translation);
	vector3d<T> getTranslation() const { return vector3d<T>(_30, _31, _32); }

	CMatrix4<T>& scale(const vector3d<T>& scale);
	CMatrix4<T>& scale(const T scale) { return scale(vector3d<T>(scale, scale, scale)); }
	CMatrix4<T>& setScale(const vector3d<T>& scale);
	vector3d<T> getScale() const;

	vector3d<T> getDir() const { return f3d::normalize(getRow(2)); }
	vector3d<T> getUp() const { return f3d::normalize(getRow(1)); }
	vector3d<T> getRight() const { return f3d::normalize(getRow(0)); }

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
			T _00, _01, _02, _03;
			T _10, _11, _12, _13;
			T _20, _21, _22, _23;
			T _30, _31, _32, _33;
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

	_00 = vX.x;
	_01 = vX.y;
	_02 = vX.z;
	_03 = (T)0.0f;

	_10 = vY.x;
	_11 = vY.y;
	_12 = vY.z;
	_13 = (T)0.0f;

	_20 = vZ.x;
	_21 = vZ.y;
	_22 = vZ.z;
	_23 = (T)0.0f;

	_30 = vPos.x;
	_31 = vPos.y;
	_32 = vPos.z;
	_33 = (T)1.0f;

	return *this;
}

template <class T>
inline void CMatrix4<T>::transpose()
{
	T tmp;
	tmp = _01;
	_01 = _10;
	_10 = tmp;

	tmp = _02;
	_02 = _20;
	_20 = tmp;

	tmp = _03;
	_03 = _30;
	_30 = tmp;

	tmp = _12;
	_12 = _21;
	_21 = tmp;

	tmp = _13;
	_13 = _31;
	_31 = tmp;

	tmp = _23;
	_23 = _32;
	_32 = tmp;
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

	res._00 = lhs._00 * rhs._00 + lhs._01 * rhs._10 + lhs._02 * rhs._20 + lhs._03 * rhs._30;
	res._01 = lhs._00 * rhs._01 + lhs._01 * rhs._11 + lhs._02 * rhs._21 + lhs._03 * rhs._31;
	res._02 = lhs._00 * rhs._02 + lhs._01 * rhs._12 + lhs._02 * rhs._22 + lhs._03 * rhs._32;
	res._03 = lhs._00 * rhs._03 + lhs._01 * rhs._13 + lhs._02 * rhs._23 + lhs._03 * rhs._33;

	res._10 = lhs._10 * rhs._00 + lhs._11 * rhs._10 + lhs._12 * rhs._20 + lhs._13 * rhs._30;
	res._11 = lhs._10 * rhs._01 + lhs._11 * rhs._11 + lhs._12 * rhs._21 + lhs._13 * rhs._31;
	res._12 = lhs._10 * rhs._02 + lhs._11 * rhs._12 + lhs._12 * rhs._22 + lhs._13 * rhs._32;
	res._13 = lhs._10 * rhs._03 + lhs._11 * rhs._13 + lhs._12 * rhs._23 + lhs._13 * rhs._33;

	res._20 = lhs._20 * rhs._00 + lhs._21 * rhs._10 + lhs._22 * rhs._20 + lhs._23 * rhs._30;
	res._21 = lhs._20 * rhs._01 + lhs._21 * rhs._11 + lhs._22 * rhs._21 + lhs._23 * rhs._31;
	res._22 = lhs._20 * rhs._02 + lhs._21 * rhs._12 + lhs._22 * rhs._22 + lhs._23 * rhs._32;
	res._23 = lhs._20 * rhs._03 + lhs._21 * rhs._13 + lhs._22 * rhs._23 + lhs._23 * rhs._33;

	res._30 = lhs._30 * rhs._00 + lhs._31 * rhs._10 + lhs._32 * rhs._20 + lhs._33 * rhs._30;
	res._31 = lhs._30 * rhs._01 + lhs._31 * rhs._11 + lhs._32 * rhs._21 + lhs._33 * rhs._31;
	res._32 = lhs._30 * rhs._02 + lhs._31 * rhs._12 + lhs._32 * rhs._22 + lhs._33 * rhs._32;
	res._33 = lhs._30 * rhs._03 + lhs._31 * rhs._13 + lhs._32 * rhs._23 + lhs._33 * rhs._33;

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

	_00 = lhs._00 * rhs._00 + lhs._01 * rhs._10 + lhs._02 * rhs._20 + lhs._03 * rhs._30;
	_01 = lhs._00 * rhs._01 + lhs._01 * rhs._11 + lhs._02 * rhs._21 + lhs._03 * rhs._31;
	_02 = lhs._00 * rhs._02 + lhs._01 * rhs._12 + lhs._02 * rhs._22 + lhs._03 * rhs._32;
	_03 = lhs._00 * rhs._03 + lhs._01 * rhs._13 + lhs._02 * rhs._23 + lhs._03 * rhs._33;

	_10 = lhs._10 * rhs._00 + lhs._11 * rhs._10 + lhs._12 * rhs._20 + lhs._13 * rhs._30;
	_11 = lhs._10 * rhs._01 + lhs._11 * rhs._11 + lhs._12 * rhs._21 + lhs._13 * rhs._31;
	_12 = lhs._10 * rhs._02 + lhs._11 * rhs._12 + lhs._12 * rhs._22 + lhs._13 * rhs._32;
	_13 = lhs._10 * rhs._03 + lhs._11 * rhs._13 + lhs._12 * rhs._23 + lhs._13 * rhs._33;

	_20 = lhs._20 * rhs._00 + lhs._21 * rhs._10 + lhs._22 * rhs._20 + lhs._23 * rhs._30;
	_21 = lhs._20 * rhs._01 + lhs._21 * rhs._11 + lhs._22 * rhs._21 + lhs._23 * rhs._31;
	_22 = lhs._20 * rhs._02 + lhs._21 * rhs._12 + lhs._22 * rhs._22 + lhs._23 * rhs._32;
	_23 = lhs._20 * rhs._03 + lhs._21 * rhs._13 + lhs._22 * rhs._23 + lhs._23 * rhs._33;

	_30 = lhs._30 * rhs._00 + lhs._31 * rhs._10 + lhs._32 * rhs._20 + lhs._33 * rhs._30;
	_31 = lhs._30 * rhs._01 + lhs._31 * rhs._11 + lhs._32 * rhs._21 + lhs._33 * rhs._31;
	_32 = lhs._30 * rhs._02 + lhs._31 * rhs._12 + lhs._32 * rhs._22 + lhs._33 * rhs._32;
	_33 = lhs._30 * rhs._03 + lhs._31 * rhs._13 + lhs._32 * rhs._23 + lhs._33 * rhs._33;

	return *this;
}

template <class T>
inline float CMatrix4<T>::getDeterminant() const
{
	float fDet;
	fDet = _00 * _11 * _22 * _33;
	fDet += _10 * _21 * _32 * _03;
	fDet += _20 * _31 * _02 * _13;
	fDet += _30 * _01 * _12 * _23;
	fDet -= _30 * _21 * _12 * _03;
	fDet -= _31 * _22 * _13 * _00;
	fDet -= _32 * _23 * _10 * _01;
	fDet -= _33 * _20 * _11 * _02;

	return fDet;
}

template <class T>
inline CMatrix4<T> CMatrix4<T>::getInverse() const
{
	CMatrix4<T> out;

	float d = getDeterminant();
	d = reciprocal_(d);

	out._00 = Det3x3(_11, _12, _13, _21, _22, _23, _31, _32, _33) * d;
	out._01 = -Det3x3(_01, _02, _03, _21, _22, _23, _31, _32, _33) * d;
	out._02 = Det3x3(_01, _02, _03, _11, _12, _13, _31, _32, _33) * d;
	out._03 = -Det3x3(_01, _02, _03, _11, _12, _13, _21, _22, _23) * d;

	out._10 = -Det3x3(_10, _12, _13, _20, _22, _23, _30, _32, _33) * d;
	out._11 = Det3x3(_00, _02, _03, _20, _22, _23, _30, _32, _33) * d;
	out._12 = -Det3x3(_00, _02, _03, _10, _12, _13, _30, _32, _33) * d;
	out._13 = Det3x3(_00, _02, _03, _10, _12, _13, _20, _22, _23) * d;

	out._20 = Det3x3(_10, _11, _13, _20, _21, _23, _30, _31, _33) * d;
	out._21 = -Det3x3(_00, _01, _03, _20, _21, _23, _30, _31, _33) * d;
	out._22 = Det3x3(_00, _01, _03, _10, _11, _13, _30, _31, _33) * d;
	out._23 = -Det3x3(_00, _01, _03, _10, _11, _13, _20, _21, _23) * d;

	out._30 = -Det3x3(_10, _11, _12, _20, _21, _22, _30, _31, _32) * d;
	out._31 = Det3x3(_00, _01, _02, _20, _21, _22, _30, _31, _32) * d;
	out._32 = -Det3x3(_00, _01, _02, _10, _11, _12, _30, _31, _32) * d;
	out._33 = Det3x3(_00, _01, _02, _10, _11, _12, _20, _21, _22) * d;

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
inline vector3df CMatrix4<T>::multiplyPoint(const vector3df& vect, float& z) const
{
	vector3df vector;

	vector.x = vect.x * _00 + vect.y * _10 + vect.z * _20 + _30;
	vector.y = vect.x * _01 + vect.y * _11 + vect.z * _21 + _31;
	vector.z = vect.x * _02 + vect.y * _12 + vect.z * _22 + _32;
	z = vect.x * _03 + vect.y * _13 + vect.z * _23 + _33;

	return vector;
}

template <class T>
inline vector3df CMatrix4<T>::multiplyPoint(const vector3df& vect) const
{
	vector3df vector;

	vector.x = vect.x * _00 + vect.y * _10 + vect.z * _20 + _30;
	vector.y = vect.x * _01 + vect.y * _11 + vect.z * _21 + _31;
	vector.z = vect.x * _02 + vect.y * _12 + vect.z * _22 + _32;

	return vector;
}

template <class T>
inline vector3df CMatrix4<T>::multiplyVector(const vector3df& vect) const
{
	vector3df vector;

	vector.x = vect.x * _00 + vect.y * _10 + vect.z * _20;
	vector.y = vect.x * _01 + vect.y * _11 + vect.z * _21;
	vector.z = vect.x * _02 + vect.y * _12 + vect.z * _22;

	return vector;
}

template <class T>
inline plane3df CMatrix4<T>::transformPlane(const plane3df& plane) const
{
	vector3df point = multiplyPoint(plane.getMemberPoint());

	CMatrix4<T> m = getInverse();
	m.transpose();

	vector3df normal = m.multiplyPoint(plane.Normal);

	return plane3df(point, normal);
}

template <class T>
inline aabbox3df CMatrix4<T>::transformBox(const aabbox3df& box) const
{
	vector3df vCenter = multiplyPoint(box.getCenter());

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
	tmp.x = vect.x * _00 + vect.y * _10 + vect.z * _20;
	tmp.y = vect.x * _01 + vect.y * _11 + vect.z * _21;
	tmp.z = vect.x * _02 + vect.y * _12 + vect.z * _22;
	return tmp;
}

template <class T>
inline  vector3df CMatrix4<T>::inverseRotateVector(const vector3df& vect) const
{
	vector3df tmp;
	tmp.x = vect.x * _00 + vect.y * _01 + vect.z * _02;
	tmp.y = vect.x * _10 + vect.y * _11 + vect.z * _12;
	tmp.z = vect.x * _20 + vect.y * _21 + vect.z * _22;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setTranslation(const vector3d<T>& translation)
{
	_30 = translation.x;
	_31 = translation.y;
	_32 = translation.z;
	return *this;
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::scale(const vector3d<T>& scale)
{
	ASSERT(scale.x > 0);			// 0
	ASSERT(scale.y > 0);			// 5
	ASSERT(scale.z > 0);			// 10

	_00 *= scale.x;
	_10 *= scale.x;
	_20 *= scale.x;
	_30 *= scale.x;

	_01 *= scale.y;
	_11 *= scale.y;
	_21 *= scale.y;
	_31 *= scale.y;

	_02 *= scale.z;
	_12 *= scale.z;
	_22 *= scale.z;
	_32 *= scale.z;

	return (*this);
}

template <class T>
inline CMatrix4<T>& CMatrix4<T>::setScale(const vector3d<T>& scale)
{
	_00 = scale.x;	_01 = 0; _02 = 0; _03 = 0;
	_00 = 0;	_01 = scale.y; _02 = 0; _03 = 0;
	_00 = 0;	_01 = 0; _02 = scale.z; _03 = 0;
	_30 = 0;	_31 = 0; _32 = 0; _33 = 1;

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

typedef CMatrix4<float> matrix4;
