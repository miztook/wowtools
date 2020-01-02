#pragma once

#include "function.h"

template <class T>
class vector4d
{
public:
	//
	vector4d() : x(0), y(0), z(0), w(0) {}
	vector4d(T nx, T ny, T nz, T nw) : x(nx), y(ny), z(nz), w(nw) {}
	explicit vector4d(T n) : x(n), y(n), z(n), w(n) {}
	vector4d(const vector4d<T>& other) { *this = other; }

	//
	vector4d<T> operator-() const { return vector4d<T>(-x, -y, -z, -w); }
	vector4d<T>& operator=(const vector4d<T>& other)
	{
		x = other.x; y = other.y; z = other.z; w = other.w;
		return *this;
	}

	vector4d<T> operator+(const vector4d<T>& other) const { return vector4d<T>(x + other.x, y + other.y, z + other.z, w + other.w); }
	vector4d<T>& operator+=(const vector4d<T>& other) { x += other.x; y += other.y; z += other.z; w += other.w; return *this; }

	vector4d<T> operator-(const vector4d<T>& other) const { return vector4d<T>(x - other.x, y - other.y, z - other.z, w - other.w); }
	vector4d<T>& operator-=(const vector4d<T>& other) { x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }

	vector4d<T> operator*(const vector4d<T>& other) const { return vector4d<T>(x * other.x, y * other.y, z * other.z, w * other.w); }
	vector4d<T>& operator*=(const vector4d<T>& other) { x *= other.x; y *= other.y; z *= other.z; w *= other.w; return *this; }
	vector4d<T> operator*(const T v) const { return vector4d<T>(x * v, y * v, z * v, w * v); }
	vector4d<T>& operator*=(const T v) { x *= v; y *= v; z *= v; w *= v; return *this; }

	vector4d<T> operator/(const vector4d<T>& other) const { return vector4d<T>(x / other.x, y / other.y, z / other.z, w / other.w); }
	vector4d<T>& operator/=(const vector4d<T>& other) { x /= other.x; y /= other.y; z /= other.z; w /= other.w; return *this; }
	vector4d<T> operator/(const T v) const { return vector4d<T>(x / v, y / v, z / v, w / v); }
	vector4d<T>& operator/=(const T v) { x /= v; y /= v; z /= v; w /= v; return *this; }

	bool operator==(const vector4d<T>& other) const
	{
		if (this == &other) return true;
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool operator!=(const vector4d<T>& other) const { return !(*this == other); }

	//
	bool equals(const vector4d<T>& other) const { return equals_(x, other.x) && equals_(y, other.y) && equals_(z, other.z) && equals_(w, other.w); }

	void set(T nx, T ny, T nz, T nw) { x = nx; y = ny; z = nz; w = nw; }
	void set(const vector4d<T>& p) { x = p.x; y = p.y; z = p.z; w = p.w; }
	void clear() { memset(&x, 0, sizeof(T) * 4); }

	T magnitude() const { return squareroot_(x*x + y*y + z*z + w*w); }
	T squareMagnitude() const { return x*x + y*y + z*z + w*w; }
	T dotProduct(const vector4d<T>& other) const { return x*other.x + y*other.y + z*other.z + w*other.w; }

	static const vector4d<T>& One() { static vector4d<T> m(1); return m; }
	static const vector4d<T>& Zero() { static vector4d<T> m(0); return m; }

	vector4d<T>& normalize()
	{
		float length = (float)(x*x + y*y + z*z + w*w);
		if (equals_(length, 0.f))
			return *this;
		length = reciprocal_squareroot_(length);
		x = (T)(x * length);
		y = (T)(y * length);
		z = (T)(z * length);
		w = (T)(w * length);
		return *this;
	}

	static vector4d<T> interpolate(const vector4d<T>& a, const vector4d<T>& b, float d)
	{
		float inv = 1.0f - d;
		return vector4d<T>((a.x*inv + b.x*d), (a.y*inv + b.y*d), (a.z*inv + b.z*d), (a.w*inv + b.w*d));
	}

public:
	T x, y, z, w;
};

typedef vector4d<float> vector4df;
typedef vector4d<int32_t> vector4di;