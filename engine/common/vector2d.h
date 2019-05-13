#pragma once

#include "function.h"

template <class T>
class vector2d
{
public:
	//
	 vector2d() : x(0), y(0) {}
	 vector2d(T nx, T ny) : x(nx), y(ny) {}
	 explicit vector2d(T n) : x(n), y(n) {}
	 vector2d(const vector2d<T>& other) { *this = other; }

	//
	 vector2d<T> operator-() const { return vector2d<T>(-x, -y); }
	 vector2d<T>& operator=(const vector2d<T>& other)
	 { 
		x = other.x; y = other.y;
			return *this;
	 }

	 vector2d<T> operator+(const vector2d<T>& other) const { return vector2d<T>(x + other.x, y + other.y); }
	 vector2d<T>& operator+=(const vector2d<T>& other) { x+=other.x; y+=other.y; return *this; }

	 vector2d<T> operator-(const vector2d<T>& other) const { return vector2d<T>(x - other.x, y - other.y); }
	 vector2d<T>& operator-=(const vector2d<T>& other) { x-=other.x; y-=other.y; return *this; }

	 vector2d<T> operator*(const vector2d<T>& other) const { return vector2d<T>(x * other.x, y * other.y); }
	 vector2d<T>& operator*=(const vector2d<T>& other) { x*=other.x; y*=other.y; return *this; }
	 vector2d<T> operator*(const T v) const { return vector2d<T>(x * v, y * v); }
	 vector2d<T>& operator*=(const T v) { x*=v; y*=v; return *this; }

	 vector2d<T> operator/(const vector2d<T>& other) const { return vector2d<T>(x / other.x, y / other.y); }
	 vector2d<T>& operator/=(const vector2d<T>& other) { x/=other.x; y/=other.y; return *this; }
	 vector2d<T> operator/(const T v) const { return vector2d<T>(x / v, y / v); }
	 vector2d<T>& operator/=(const T v) { x/=v; y/=v; return *this; }

	bool operator==(const vector2d<T>& other) const 
	{
		if ( this == &other ) return true;
		return x == other.x && y == other.y;
	}

	 bool operator!=(const vector2d<T>& other) const { return !(*this == other ); }

	//
	 bool equals(const vector2d<T>& other) const { return equals_(x, other.x) && equals_(y, other.y); }

	 void set(T nx, T ny) {x=nx; y=ny;}
	 void set(const vector2d<T>& p) { x=p.x; y=p.y; }
	 void clear() { memset(&x,0,sizeof(T)*2); }

	 T magnitude() const { return squareroot_( x*x + y*y ); }
	 T squareMagnitude() const { return x*x + y*y; }
	 T dotProduct(const vector2d<T>& other) const { return x*other.x + y*other.y; }

	 T getDistanceFrom(const vector2d<T>& other) const { return vector2d<T>(x - other.x, y - other.y).magnitude(); }
	 T getDistanceFromSQ(const vector2d<T>& other) const { return vector2d<T>(x - other.x, y - other.y).squareMagnitude(); }

	 vector2d<T>& normalize()
	 {
		 float length = (float)(x*x + y*y);
		 if (equals_(length, 0.f))
			 return *this;
		 length = reciprocal_squareroot_(length);
		 x = (T)(x * length);
		 y = (T)(y * length);
		 return *this;
	 }

	static vector2d<T> interpolate(const vector2d<T>& a, const vector2d<T>& b, float d)
	{
		float inv = 1.0f - d;
		return vector2d<T>((a.x*inv + b.x*d), (a.y*inv + b.y*d));
	}

	vector2d<T> getInterpolated_quadratic(const vector2d<T>& v2, const vector2d<T>& v3, float d) const
	{
		// this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
		const float inv = 1.0f - d;
		const float mul0 = inv * inv;
		const float mul1 = 2.0f * d * inv;
		const float mul2 = d * d;

		return vector2d<T> ( (T)(x * mul0 + v2.x * mul1 + v3.x * mul2),
			(T)(y * mul0 + v2.y * mul1 + v3.y * mul2));
	}

	T getArea() const { return x * y; }

	vector2d<T> getMipLevelSize(uint32_t level) const
	{
		return vector2d<T>(max_(1u, x >> level), max_(1u, y >> level));
	}

	uint32_t getNumMipLevels() const
	{
		uint32_t mip = 0;
		uint32_t len = min_(x, y);
		while (len)
		{
			len >>= 1;
			++mip;
		}

		return mip;
	}

public:
	union
	{
		struct
		{
			T x, y;
		};

		struct
		{
			T width, height;
		};
	};
};

typedef vector2d<float> vector2df;
typedef vector2d<int32_t> vector2di;
typedef vector2d<uint32_t> dimension2d;

