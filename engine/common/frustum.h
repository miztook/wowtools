#pragma once

#include <memory.h>
#include "plane3d.h"
#include "matrix4.h"
#include "vector3d.h"


class frustum
{
public:
	enum : int
	{
		VF_LEFT = 0,
		VF_RIGHT,
		VF_TOP,
		VF_BOTTOM,
		VF_NEAR,
		VF_FAR,

		VF_PLANE_COUNT,
	};

public:
	frustum() {}

	frustum(const frustum& other) { (*this) = other; }

public:
	frustum& operator=(const frustum& other)
	{
		ASSERT(this != &other);
		for (uint32_t i = 0; i < VF_PLANE_COUNT; ++i)
			planes[i] = other.planes[i];
		return *this;
	}

	void transform(const matrix4& mat);

	bool isInFrustum(const aabbox3df& box) const;
	bool isInFrustum(const vector3df& pos) const;

	bool operator==(const frustum& other) const { return equals(other); }
	bool operator!=(const frustum& other) const { return !(*this == other); }

	const plane3df& getPlane(int index) const { return planes[index]; }
	void setPlane(int index, const plane3df& p) { planes[index] = p; }

	bool equals(const frustum& other) const;

private:
	plane3df		planes[VF_PLANE_COUNT];
};

inline void frustum::transform(const matrix4& mat)
{
	for (uint32_t i = 0; i < VF_PLANE_COUNT; ++i)
		planes[i] = mat.transformPlane(planes[i]);
}


inline bool frustum::isInFrustum(const aabbox3df& box) const
{
	for (int p = 1; p < 6; ++p)
	{
		if (ISREL3D_BACK == box.classifyPlaneRelation(planes[p]))
			return false;
	}

	return true;
}

inline bool frustum::isInFrustum(const vector3df& pos) const
{
	for (int p = 1; p < 6; ++p)
	{
		if (ISREL3D_BACK == planes[p].classifyPointRelation(pos))
			return false;
	}

	return true;
}

inline bool frustum::equals(const frustum& other) const
{
	for (int i = 0; i < VF_PLANE_COUNT; ++i)
	{
		if (planes[i] != other.planes[i])
			return false;
	}
	return true;
}
