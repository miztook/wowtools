#pragma once

#include "vector2d.h"

template <class T>
class rect
{
public:
	//
	 rect() {}
	 rect(T x, T y, T x2, T y2) : UpperLeftCorner(x,y), LowerRightCorner(x2,y2) {}
	 rect(const vector2d<T>& upperLeft, const vector2d<T>& lowerRight) : UpperLeftCorner(upperLeft), LowerRightCorner(lowerRight) {}
	 rect(const rect<T>& other) { *this = other; }

	 void set(T x, T y, T x2, T y2) { UpperLeftCorner.set(x, y); LowerRightCorner.set(x2, y2); }
	 void set(const vector2d<T>& upperLeft, const vector2d<T>& lowerRight) { UpperLeftCorner = upperLeft; LowerRightCorner = lowerRight; };

	 bool isEmpty() const { return UpperLeftCorner.x >= LowerRightCorner.x || UpperLeftCorner.y >= LowerRightCorner.y; }

	//
	 rect& operator=(const rect<T>& other)
	 {
		 ASSERT(this != &other);
		 UpperLeftCorner = other.UpperLeftCorner;
		 LowerRightCorner = other.LowerRightCorner;
		 return *this;
	 }
	 T getWidth() const { return LowerRightCorner.x - UpperLeftCorner.x; }
	 T getHeight() const { return LowerRightCorner.y - UpperLeftCorner.y; }
	 vector2d<T> getSize() const { return vector2d<T>(getWidth(), getHeight()); }

	void clipAgainst(const rect<T>& other);
	bool contains(const rect<T>& other) const;
	bool intersectsWithRect(const rect<T>& other) const;

	bool operator==(const rect<T>& other) const
	{
		return UpperLeftCorner == other.UpperLeftCorner && LowerRightCorner == other.LowerRightCorner;
	}

	bool operator!=(const rect<T>& other) const
	{
		return UpperLeftCorner != other.UpperLeftCorner || LowerRightCorner != other.LowerRightCorner;
	}

	rect<T> operator*(const T v) const { return rect<T>(UpperLeftCorner * v, LowerRightCorner * v); }
	rect<T>& operator*=(const T v) { UpperLeftCorner*=v; LowerRightCorner*=v; return *this; }

public:
	union
	{
		
		vector2d<T>		UpperLeftCorner;
		vector2d<T>		LowerRightCorner;
		struct  
		{
			T left;
			T top;
			T right;
			T bottom;
		};
	};
	
};

template <class T>
inline bool rect<T>::contains( const rect<T>& other ) const
{
	return (other.LowerRightCorner.x <= LowerRightCorner.x && 
		other.LowerRightCorner.y <= LowerRightCorner.y &&
		other.UpperLeftCorner.x >= UpperLeftCorner.x &&
		other.UpperLeftCorner.y >= UpperLeftCorner.y );
}

template <class T>
inline bool rect<T>::intersectsWithRect( const rect<T>& other ) const
{
	return ( UpperLeftCorner.x < other.LowerRightCorner.x && 
		UpperLeftCorner.y < other.LowerRightCorner.y &&
		LowerRightCorner.x > other.UpperLeftCorner.x &&
		LowerRightCorner.y > other.UpperLeftCorner.y );
}

template <class T>
inline void rect<T>::clipAgainst(const rect<T>& other)
{
	if (other.LowerRightCorner.x < LowerRightCorner.x)
		LowerRightCorner.x = other.LowerRightCorner.x;
	if (other.LowerRightCorner.y < LowerRightCorner.y)
		LowerRightCorner.y = other.LowerRightCorner.y;

	if (other.UpperLeftCorner.x > UpperLeftCorner.x)
		UpperLeftCorner.x = other.UpperLeftCorner.x;
	if (other.UpperLeftCorner.y > UpperLeftCorner.y)
		UpperLeftCorner.y = other.UpperLeftCorner.y;

	// correct possible invalid rect resulting from clipping
	if (UpperLeftCorner.y > LowerRightCorner.y)
		UpperLeftCorner.y = LowerRightCorner.y;
	if (UpperLeftCorner.x > LowerRightCorner.x)
		UpperLeftCorner.x = LowerRightCorner.x;
}

typedef rect<float> rectf;
typedef rect<int32_t> recti;
