#pragma once

#include "function.h"

template <class T>
class vector3d
{
public:
	//
	 vector3d() : x(0), y(0), z(0){}
	 vector3d(T nx, T ny, T nz) : x(nx), y(ny), z(nz) {}
	 explicit vector3d(T n) : x(n), y(n), z(n) {}
	 vector3d(const vector3d<T>& other) { (*this) = other; }

	//
	 vector3d<T> operator-() const { return vector3d<T>(-x, -y, -z); }

	 vector3d<T>& operator=(const vector3d<T>& other) 
	 {
		x = other.x; 
		y = other.y; 
		z = other.z; 
		return *this; 
	 }

	 vector3d<T> operator+(const vector3d<T>& other) const { return vector3d<T>(x + other.x, y + other.y, z + other.z); }
	 vector3d<T>& operator+=(const vector3d<T>& other) { x+=other.x; y+=other.y; z+=other.z; return *this; }

	 vector3d<T> operator-(const vector3d<T>& other) const { return vector3d<T>(x - other.x, y - other.y, z - other.z); }
	 vector3d<T>& operator-=(const vector3d<T>& other) { x-=other.x; y-=other.y; z-=other.z; return *this; }

	 vector3d<T> operator*(const vector3d<T>& other) const { return vector3d<T>(x * other.x, y * other.y, z * other.z); }
	 vector3d<T>& operator*=(const vector3d<T>& other) { x*=other.x; y*=other.y; z*=other.z; return *this; }
	 vector3d<T> operator*(const T v) const { return vector3d<T>(x * v, y * v, z * v); }
	 vector3d<T>& operator*=(const T v) { x*=v; y*=v; z*=v; return *this; }

	 vector3d<T> operator/(const vector3d<T>& other) const { return vector3d<T>(x / other.x, y / other.y, z / other.z); }
	 vector3d<T>& operator/=(const vector3d<T>& other) { x/=other.x; y/=other.y; z/=other.z; return *this; }
	 vector3d<T> operator/(const T v) const { T i=(T)1.0/v; return vector3d<T>(x * i, y * i, z * i); }
	 vector3d<T>& operator/=(const T v) { T i=(T)1.0/v; x*=i; y*=i; z*=i; return *this; }

	bool operator==(const vector3d<T>& other) const
	{
		if ( this == &other ) return true;
		return x == other.x && y == other.y && z == other.z;
	}

	 bool operator!=(const vector3d<T>& other) const { return !(*this == other );	}

	//
	 bool equals(const vector3d<T>& other, const T tolerance ) const
	 {
		 return equals_(x, other.x, tolerance) &&
			 equals_(y, other.y, tolerance) &&
			 equals_(z, other.z, tolerance);
	 }

	 void set(const T nx, const T ny, const T nz) {x=nx; y=ny; z=nz;}
	 void set(const vector3d<T>& p) {x=p.x; y=p.y; z=p.z;}
	 void clear() { memset(&x,0,sizeof(T)*3); }

	 T magnitude() const { return squareroot_( x*x + y*y + z*z ); }
	 T squareMagnitude() const { return x*x + y*y + z*z; }

	 T dotProduct(const vector3d<T>& other) const { return x*other.x + y*other.y + z*other.z; }
	 vector3d<T> crossProduct(const vector3d<T>& p) const { return vector3d<T>(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x); }

	 T getDistanceFrom(const vector3d<T>& other) const { return vector3d<T>(x - other.x, y - other.y, z - other.z).magnitude(); }
	 T getDistanceFromSQ(const vector3d<T>& other) const { return vector3d<T>(x - other.x, y - other.y, z - other.z).squareMagnitude(); }

	 bool isZero() const { return (*this) == Zero(); }

	 vector3d<T> getXZ() const { vector3d<T> v(x, 0, z); v.normalize(); return v; }

	 static const vector3d<T>& One() { static vector3d<T> m(1); return m; }
	 static const vector3d<T>& Zero() { static vector3d<T> m(0); return m; }
	 static const vector3d<T>& UnitX() { static vector3d<T> m(1,0,0); return m; }
	 static const vector3d<T>& UnitY() { static vector3d<T> m(0,1,0); return m; }
	 static const vector3d<T>& UnitZ() { static vector3d<T> m(0,0,1); return m; }

	vector3d<T>& normalize()
	{
		float length = x*x + y*y + z*z;
		if (equals_(length, 0.0) || equals_(length, 1.0)) // this check isn't an optimization but prevents getting NAN in the sqrt.
			return *this;
		length = reciprocal_squareroot_(length);

		x = (T)(x * length);
		y = (T)(y * length);
		z = (T)(z * length);
		return *this;
	}

	 static vector3d<T> interpolate(const vector3d<T>& a, const vector3d<T>& b, float d)
	 {
		 float inv = 1.0f - d;
		 return vector3d<T>((a.x*inv + b.x*d), (a.y*inv + b.y*d), (a.z*inv + b.z*d));
	 }

	vector3d<T> getInterpolated_quadratic(const vector3d<T>& v2, const vector3d<T>& v3, float d) const
	{
		// this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
		const float inv = (T) 1.0 - d;
		const float mul0 = inv * inv;
		const float mul1 = (T) 2.0 * d * inv;
		const float mul2 = d * d;

		return vector3d<T> ((T)(x * mul0 + v2.x * mul1 + v3.x * mul2),
			(T)(y * mul0 + v2.y * mul1 + v3.y * mul2),
			(T)(z * mul0 + v2.z * mul1 + v3.z * mul2));
	}

public:
	T x;
	T y;
	T z;
};


typedef vector3d<float> vector3df;
typedef vector3d<int32_t> vector3di;
