#pragma once

#include "base.h"
#include "plane3d.h"
#include "line3d.h"

static const uint16_t g_aabboxLineIndex[] = { 5,1, 1,3, 3,7, 7,5, 
		0,2, 2,6, 6,4, 4,0, 
		1,0, 3,2, 7,6, 5,4 };

template <class T>
class aabbox3d
{
public:
	 aabbox3d() { clear(); }
	 aabbox3d(const vector3d<T>& min, const vector3d<T>& max): MinEdge(min), MaxEdge(max) { }	
	 explicit aabbox3d(const vector3d<T>& init): MinEdge(init), MaxEdge(init) { }
	 aabbox3d(T minx, T miny, T minz, T maxx, T maxy, T maxz): MinEdge(minx, miny, minz), MaxEdge(maxx, maxy, maxz) { }
	 aabbox3d(const aabbox3d<T>& other) { (*this) = other; }

	public:		//operator
	 bool operator==(const aabbox3d<T>& other) const { return MinEdge == other.MinEdge && MaxEdge == other.MaxEdge; }
	 bool operator!=(const aabbox3d<T>& other) const { return !(*this == other ); }

	aabbox3d& operator=(const aabbox3d<T>& other) 
	{
		ASSERT(this != &other);
		MinEdge = other.MinEdge;
		MaxEdge = other.MaxEdge;
		return *this;
	}
	public:

	//intersection
	 EIntersectionRelation3D classifyPlaneRelation(const plane3d<T>& plane) const;
			
	 void setByMinMax(const vector3d<T>& min, const vector3d<T>& max) { MinEdge=min; MaxEdge=max; }
	 void setByCenterExtent(const vector3d<T>& center, const vector3d<T>& ext) { MinEdge = center - ext; MaxEdge = center + ext; }
	 void clear() {
		 MinEdge.set(999999.0f, 999999.0f, 999999.0f);
		 MaxEdge.set(-999999.0f, -999999.0f, -999999.0f);
	 }
	 void addInternalPoint( const vector3d<T>& p );
	 void addInternalBox( const aabbox3d<T>& b );

	 bool isZero() const { return MaxEdge == MinEdge; }

	 bool isValid() const { return MaxEdge.x >= MinEdge.x && MaxEdge.y >= MinEdge.y && MaxEdge.z >= MinEdge.z; }

	 vector3d<T> getCenter() const { return (MinEdge + MaxEdge)*0.5f; }
	 vector3d<T> getExtent() const { return MaxEdge - MinEdge; }
	 T getVolume() const { const vector3d<T> e = getExtent(); return e.x * e.y * e.z; }
	 aabbox3d<T>	getInterpolated( const aabbox3d<T>& other, float d ) const;

	bool isPointInside(const vector3d<T>& p) const
	{
		return (p.x >= MinEdge.x && p.x <= MaxEdge.x &&
			p.y >= MinEdge.y && p.y <= MaxEdge.y &&
			p.z >= MinEdge.z && p.z <= MaxEdge.z);
	}

	bool intersectsWithBox(const aabbox3d<T>& other) const
	{
		return (MinEdge.x <= other.MaxEdge.x && MinEdge.y <= other.MaxEdge.y && MinEdge.z <= other.MaxEdge.z &&
			MaxEdge.x >= other.MinEdge.x && MaxEdge.y >= other.MinEdge.y && MaxEdge.z >= other.MinEdge.z);
	}

	bool intersectsWithLine(const line3d<T>& line) const
	{
		return intersectsWithLine(line.getMiddle(), line.getVector().normalize(),
			(T)(line.getLength() * 0.5));
	}

	bool intersectsWithLine(const vector3d<T>& linemiddle,
		const vector3d<T>& linevect, T halflength) const;

	static const aabbox3d<T>& Zero() { static aabbox3d<T> m; return m; }

	void makePoints(vector3d<T>* points) const;
	void getVertices(vector3d<T>* points, uint16_t* aIndices, bool bWire) const;

public:
	vector3d<T>	MinEdge;			//a
	vector3d<T>	MaxEdge;			//b
};

template <class T>
void aabbox3d<T>::getVertices(vector3d<T>* points, uint16_t* aIndices, bool bWire) const
{
	obbox3d<T> obb;

	obb.Center	= getCenter();
	obb.Extents = getExtent();
	obb.XAxis.set(1.0f, 0.0f, 0.0f);
	obb.YAxis.set(0.0f, 1.0f, 0.0f);
	obb.ZAxis.set(0.0f, 0.0f, 1.0f);
	obb.completeExtAxis();

	obb.getVertices(points, aIndices, bWire);
}

template <class T>
inline void aabbox3d<T>::makePoints(vector3d<T>* points) const
{
	vector3d<T> center = getCenter();
	vector3d<T> ext = getExtent() / 2;

	/*
		/3--------/7
		/ |           / |
	/  |         /  |
	1---------5  |
	|  /2- - -|- -6
	| /           |  /
	|/            | /
	0---------4/
	*/

	points[0] = center + vector3d<T>(-ext.x, -ext.y, -ext.z);
	points[1] = center + vector3d<T>(-ext.x, ext.y, -ext.z);
	points[2] = center + vector3d<T>(-ext.x, -ext.y, ext.z);
	points[3] = center + vector3d<T>(-ext.x, ext.y, ext.z);
	points[4] = center + vector3d<T>(ext.x, -ext.y, -ext.z);
	points[5] = center + vector3d<T>(ext.x, ext.y, -ext.z);
	points[6] = center + vector3d<T>(ext.x, -ext.y, ext.z);
	points[7] = center + vector3d<T>(ext.x, ext.y, ext.z);
}

template <class T>
inline void aabbox3d<T>::addInternalBox( const aabbox3d<T>& b )
{
	addInternalPoint(b.MaxEdge);
	addInternalPoint(b.MinEdge);
}

template <class T>
inline void aabbox3d<T>::addInternalPoint( const vector3d<T>& p )
{
	if (p.x >MaxEdge.x)		MaxEdge.x = p.x;
	if (p.y >MaxEdge.y)		MaxEdge.y = p.y;
	if (p.z >MaxEdge.z)		MaxEdge.z = p.z;

	if(p.x <MinEdge.x)		MinEdge.x = p.x;
	if(p.y <MinEdge.y)		MinEdge.y = p.y;
	if(p.z <MinEdge.z)		MinEdge.z = p.z;
}

template <class T>
inline EIntersectionRelation3D aabbox3d<T>::classifyPlaneRelation( const plane3d<T>& plane ) const
{	
	vector3d<T> nearPoint(MaxEdge);
	vector3d<T> farPoint(MinEdge);

	if (plane.Normal.x > (T)0)
	{
		nearPoint.x = MinEdge.x;
		farPoint.x = MaxEdge.x;
	}
			
	if (plane.Normal.y > (T)0)
	{
		nearPoint.y = MinEdge.y;
		farPoint.y = MaxEdge.y;
	}
			
	if (plane.Normal.z > (T)0)
	{
		nearPoint.z = MinEdge.z;
		farPoint.z = MaxEdge.z;
	}

	if (plane.Normal.dotProduct(nearPoint) + plane.D > (T)0)
		return ISREL3D_FRONT;
	else if (plane.Normal.dotProduct(farPoint) + plane.D > (T)0)
		return ISREL3D_CLIPPED;
	else
		return ISREL3D_BACK;
}

template <class T>
inline aabbox3d<T> aabbox3d<T>::getInterpolated( const aabbox3d<T>& other, float d ) const
{
	float inv = 1.0f - d;
	return aabbox3d<T>((other.MinEdge*inv) + (MinEdge*d),
		(other.MaxEdge*inv) + (MaxEdge*d));
}

template <class T>
inline bool aabbox3d<T>::intersectsWithLine( const vector3d<T>& linemiddle, const vector3d<T>& linevect, T halflength ) const
{
	const vector3d<T> e = getExtent() * (T)0.5;
	const vector3d<T> t = getCenter() - linemiddle;

	if ((fabs(t.x) > e.x + halflength * fabs(linevect.x)) ||
		(fabs(t.y) > e.y + halflength * fabs(linevect.y)) ||
		(fabs(t.z) > e.z + halflength * fabs(linevect.z)) )
		return false;

	T r = e.y * (T)fabs(linevect.z) + e.z * (T)fabs(linevect.y);
	if (fabs(t.y*linevect.z - t.z*linevect.y) > r )
		return false;

	r = e.x * (T)fabs(linevect.z) + e.z * (T)fabs(linevect.x);
	if (fabs(t.z*linevect.x - t.x*linevect.z) > r )
		return false;

	r = e.x * (T)fabs(linevect.y) + e.y * (T)fabs(linevect.x);
	if (fabs(t.x*linevect.y - t.y*linevect.x) > r)
		return false;

	return true;
}

typedef aabbox3d<float> aabbox3df;
typedef aabbox3d<int32_t> aabbox3di;

	
