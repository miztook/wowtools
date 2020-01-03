#pragma once

#include "vector3d.h"
#include "matrix4.h"

class quaternion
{
public:
	//
	 quaternion() : x(0), y(0), z(0), w(0) {}
	 quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }
	 quaternion(float pitch, float yaw, float roll) { fromEuler( pitch, yaw, roll ); }
	 explicit quaternion(const vector3df& vec) { fromEuler( vec ); }
	 quaternion(const vector3df& axis, float angle) { fromAngleAxis( angle, axis ); }
	 quaternion(const quaternion& other) { *this = other; }
	//
	quaternion& operator=(const quaternion& other)
	{
		ASSERT(this != &other);
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
		return *this;
	}

	bool equals(const quaternion& other, const float tolerance=(float)ROUNDING_ERROR_f32) const { return equals_(x, other.x, tolerance) && equals_(y, other.y, tolerance) && equals_(z, other.z, tolerance) && equals_(w, other.w, tolerance);}
	bool operator==(const quaternion& other) const 
	{
		if ( this == &other ) return true;
		return memcmp(this, &other, sizeof(quaternion)) == 0;
	}
	 bool operator!=(const quaternion& other) const { return !(*this == other ); }
	 quaternion operator+(const quaternion& other) const { return quaternion(x+other.x, y+other.y, z+other.z, w+other.w); }
	 quaternion operator*(const quaternion& other) const;
	 quaternion operator*(float s) const { return quaternion(s*x, s*y, s*z, s*w); }
	 quaternion& operator*=(float s) { x*=s; y*=s; z*=s; w*=s; return *this; }
	 vector3df operator*(const vector3df& v) const;
	 quaternion& operator*=(const quaternion& other) { return (*this = other * (*this)); }

	//
	 quaternion& set(float x, float y, float z, float w) { x = x; y = y; z = z; w = w; return *this; }
	 float dotProduct(const quaternion& other) const { return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w); }
	quaternion& fromAngleAxis (float angle, const vector3df& axis);
	void toAngleAxis (float &angle, vector3df& axis) const;
	quaternion& fromEuler( float pitch, float yaw, float roll );
	 quaternion& fromEuler( const vector3df& vec ) { return fromEuler(vec.x, vec.y, vec.z); }
	void toEuler(vector3df& euler) const;
	 quaternion& makeIdentity()	{ w = 1.f; x = 0.f; y = 0.f; z = 0.f; return *this; }
	quaternion& normalize();
	quaternion& fromMatrix(const matrix4& m);
	matrix4 toMatrix() const;				//
	static quaternion slerp( quaternion q1, quaternion q2, float interpolate );
	quaternion& rotationFromTo(const vector3df& from, const vector3df& to, const vector3df& axisOpposite);
	vector3df multiplyPoint( const vector3df& vect) const;

public:
	float x;		//imaginary
	float y;
	float z;
	float w;		//real
};

inline quaternion quaternion::operator*(const quaternion& other) const
{
	quaternion tmp;

	tmp.w = (other.w * w) - (other.x * x) - (other.y * y) - (other.z * z);
	tmp.x = (other.w * x) + (other.x * w) + (other.y * z) - (other.z * y);
	tmp.y = (other.w * y) + (other.y * w) + (other.z * x) - (other.x * z);
	tmp.z = (other.w * z) + (other.z * w) + (other.x * y) - (other.y * x);

	return tmp;
}

inline vector3df quaternion::operator*(const vector3df& v) const
{
	vector3df uv, uuv;
	vector3df qvec(x, y, z);
	uv = qvec.crossProduct(v);
	uuv = qvec.crossProduct(uv);
	uv *= (2.0f * w);
	uuv *= 2.0f;

	return v + uv + uuv;
}

inline quaternion& quaternion::fromAngleAxis (float angle, const vector3df& axis)
{
	const float fHalfAngle = 0.5f*angle;
	const float fSin = sinf(fHalfAngle);
	w = cosf(fHalfAngle);
	x = fSin*axis.x;
	y = fSin*axis.y;
	z = fSin*axis.z;
	return *this;
}

inline void quaternion::toAngleAxis (float &angle, vector3df& axis) const
{
	const float scale = sqrtf(x*x + y*y + z*z);

	if (iszero_(scale) || w > 1.0f || w < -1.0f)
	{
		angle = 0.0f;
		axis.x = 0.0f;
		axis.y = 1.0f;
		axis.z = 0.0f;
	}
	else
	{
		const float invscale = reciprocal_(scale);
		angle = 2.0f * acosf(w);
		axis.x = x * invscale;
		axis.y = y * invscale;
		axis.z = z * invscale;
	}
}

inline quaternion& quaternion::fromEuler( float pitch, float yaw, float roll )
{
	float angle;

	angle = pitch * 0.5f;
	const float sr = sin(angle);
	const float cr = cos(angle);

	angle = yaw * 0.5f;
	const float sp = sin(angle);
	const float cp = cos(angle);

	angle = roll * 0.5f;
	const float sy = sin(angle);
	const float cy = cos(angle);

	const float cpcy = cp * cy;
	const float spcy = sp * cy;
	const float cpsy = cp * sy;
	const float spsy = sp * sy;

	x = (float)(sr * cpcy - cr * spsy);
	y = (float)(cr * spcy + sr * cpsy);
	z = (float)(cr * cpsy - sr * spcy);
	w = (float)(cr * cpcy + sr * spsy);

	return normalize();
}


inline void quaternion::toEuler( vector3df& euler ) const
{
	const float sqw = w*w;
	const float sqx = x*x;
	const float sqy = y*y;
	const float sqz = z*z;

	// heading = rotation about z-axis
	euler.z = (float) (atan2(2.0f * (x*y +z*w),(sqx - sqy - sqz + sqw)));

	// bank = rotation about x-axis
	euler.x = (float) (atan2(2.0f * (y*z +x*w),(-sqx - sqy + sqz + sqw)));

	// attitude = rotation about y-axis
	euler.y = asinf( clamp_(-2.0f * (x*z - y*w), -1.0f, 1.0f) );
}


inline quaternion& quaternion::normalize()
{
	const float n = reciprocal_squareroot_( x*x + y*y + z*z + w*w );

	if (n == 1)
		return *this;

	x *= n;
	y *= n;
	z *= n;
	w *= n;

	return *this;
}

inline matrix4 quaternion::toMatrix() const
{
	matrix4 ret;

	ret._00 = 1.0f - 2.0f*y*y - 2.0f*z*z;
	ret._01 = 2.0f*x*y + 2.0f*z*w;
	ret._02 = 2.0f*x*z - 2.0f*y*w;
	ret._03 = 0.0f;

	ret._10 = 2.0f*x*y - 2.0f*z*w;
	ret._11 = 1.0f - 2.0f*x*x - 2.0f*z*z;
	ret._12 = 2.0f*z*y + 2.0f*x*w;
	ret._13 = 0.0f;

	ret._20 = 2.0f*x*z + 2.0f*y*w;
	ret._21 = 2.0f*z*y - 2.0f*x*w;
	ret._22 = 1.0f - 2.0f*x*x - 2.0f*y*y;
	ret._23 = 0.0f;

	ret._30 = 0;
	ret._31 = 0;
	ret._32 = 0;
	ret._33 = 1.f;

	return ret;
}

inline quaternion quaternion::slerp(quaternion q1, quaternion q2, float time)
{
	float angle = q1.dotProduct(q2);

	if (angle < 0.0f)
	{
		q1 *= -1.0f;
		angle *= -1.0f;
	}

	float scale;
	float invscale;

	if ((angle + 1.0f) > 0.05f)
	{
		if ((1.0f - angle) >= 0.05f) // spherical interpolation
		{
			const float theta = acosf(angle);
			const float invsintheta = reciprocal_(sinf(theta));
			scale = sinf(theta * (1.0f-time)) * invsintheta;
			invscale = sinf(theta * time) * invsintheta;
		}
		else // linear interploation
		{
			scale = 1.0f - time;
			invscale = time;
		}
	}
	else
	{
		q2.set(-q1.y, q1.x, -q1.w, q1.z);
		scale = sinf(PI * (0.5f - time));
		invscale = sinf(PI * time);
	}

	return ((q1*scale) + (q2*invscale));
}

inline quaternion& quaternion::fromMatrix( const matrix4& m )
{
	float	tr, s;

	// calculating the trace of the matrix, it is equal to 4(1 - x*x - y*y - z*z)=4w*w if it is a unit quaternion
	tr = m._00 + m._11 + m._22 + 1.0f;
	// check the diagonal
	if (tr > 0.36f) // we can calculate out w directly
	{
		s = (float)sqrt(tr); // s is 2w
		w = s * 0.5f;
		s = 0.5f / s;	// now s is 1/4w
		x = (m._12 - m._21) * s;
		y = (m._20 - m._02) * s;
		z = (m._01 - m._10) * s;
	}
	else
	{
		// we have to calculate x, y or z first
		if (m._00 >= m._11 && m._00 >= m._22)
		{
			s = (float)sqrt(1.0f + m._00 - m._11 - m._22); // s is 2x
			x = s *0.5f;
			s = 0.5f / s;
			y = (m._01 + m._10) * s;
			z = (m._02 + m._20) * s;
			w = (m._12 - m._21) * s;
		}
		else if (m._11 >= m._00 && m._11 >= m._22)
		{
			s = (float)sqrt(1.0f + m._11 - m._00 - m._22); // s is 2y
			y = s *0.5f;
			s = 0.5f / s;
			x = (m._01 + m._10) * s;
			z = (m._12 + m._21) * s;
			w = (m._20 - m._02) * s;
		}
		else // mat._33 is maximum
		{
			s = (float)sqrt(1.0f + m._22 - m._00 - m._11); // s is 2z
			z = s *0.5f;
			s = 0.5f / s;
			x = (m._02 + m._20) * s;
			y = (m._12 + m._21) * s;
			w = (m._01 - m._10) * s;
		}
	}
}

inline quaternion& quaternion::rotationFromTo( const vector3df& from, const vector3df& to, const vector3df& axisOpposite )
{
	// Based on Stan Melax's article in Game Programming Gems
	// Copy, since cannot modify local
	vector3df v0 = from;
	vector3df v1 = to;
	v0.normalize();
	v1.normalize();

	const float d = v0.dotProduct(v1);
	if (d >= 1.0f) // If dot == 1, vectors are the same
	{
		return makeIdentity();
	}
	else if (d <= -1.0f) // exactly opposite
	{
		vector3df axis = axisOpposite;
		return fromAngleAxis(PI, axis);
	}

	const float s = squareroot_( (1+d)*2 ); // optimize inv_sqrt
	const float invs = 1.f / s;
	const vector3df c = v0.crossProduct(v1)*invs;
	x = c.x;
	y = c.y;
	z = c.z;
	w = s * 0.5f;

	return *this;
}

inline vector3df quaternion::multiplyPoint( const vector3df& vect ) const
{
	matrix4 m = toMatrix();

	return m.multiplyVector(vect);
}