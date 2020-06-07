#include "CMeshManager.h"
#include "CMesh.h"
#include "Engine.h"
#include "wowEnvironment.h"
#include "wowM2File.h"

CMeshManager::CMeshManager(wowEnvironment* wowEnv)
	: WowEnv(wowEnv)
{
}

CMeshManager::~CMeshManager()
{
	for (auto itr = MeshMap.begin(); itr != MeshMap.end(); ++itr)
	{
		delete itr->second;
	}

	MeshMap.clear();
}

std::shared_ptr<wowM2File> CMeshManager::loadM2(const char* filename)
{
	if (strlen(filename) == 0)
		return nullptr;

	char realfilename[QMAX_PATH];
	normalizeFileName(filename, realfilename, QMAX_PATH);
	Q_strlwr(realfilename);

	std::shared_ptr<wowM2File> m2file = m_M2FileCache.tryLoadFromCache(realfilename);
	if (m2file)
		return m2file;

	std::shared_ptr<wowM2File> file(new wowM2File(WowEnv));
	if (!file->loadFile(realfilename))
	{
		file.reset();
		return nullptr;
	}

	if (file)
		m_M2FileCache.addToCache(realfilename, file);

	return file;
}

bool CMeshManager::addMesh(const char* name, IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, E_PRIMITIVE_TYPE primType, uint32_t primCount, const aabbox3df& box)
{
	if (MeshMap.find(name) != MeshMap.end())
		return false;

	CMesh* m = new CMesh(name, vbuffer, ibuffer, primType, primCount, box);
	MeshMap[name] = m;

	return true;
}

const CMesh* CMeshManager::getMesh(const char* name) const
{
	auto i = MeshMap.find(name);
	if (i == MeshMap.end())
		return nullptr;

	return i->second;
}

void CMeshManager::removeMesh(const char* name)
{
	auto i = MeshMap.find(name);
	if (i == MeshMap.end())
		return;

	delete i->second;

	MeshMap.erase(name);
}

bool CMeshManager::addGridLineMesh(const char* name, uint32_t xzCount, float gridSize, SColor color, SColor centerColor)
{
	uint32_t vcount = getGridLineVCount(xzCount);
	IVertexBuffer* vbuffer = g_Engine->getDriver()->createVertexBuffer(EMM_STATIC);
	{
		SVertex_PC* vertices = vbuffer->alloc<SVertex_PC>(vcount);
		fillGridLineMeshV(vertices, vcount, xzCount, gridSize, color, centerColor);
	}
	return addMesh(name, vbuffer, nullptr, EPT_LINES, vcount / 2, getGridLineAABBox(xzCount, gridSize));
}

bool CMeshManager::addPlane(const char* name, float width, float height, SColor color)
{
	IVertexBuffer* vbuffer = g_Engine->getDriver()->createVertexBuffer(EMM_STATIC);
	{
		SVertex_PCT* vertices = vbuffer->alloc<SVertex_PCT>(4);
		vertices[0].set(vector3df(-width, height, 0), color, vector2df(0, 0));
		vertices[1].set(vector3df(width, height, 0), color, vector2df(1, 0));
		vertices[2].set(vector3df(-width, -height, 0), color, vector2df(0, 1));
		vertices[3].set(vector3df(width, -height, 0), color, vector2df(1, 1));
	}

	IIndexBuffer* ibuffer = g_Engine->getDriver()->createIndexBuffer(EMM_STATIC);
	{
		uint16_t* indices = ibuffer->alloc<uint16_t>(6);
		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 2;
		indices[4] = 1;
		indices[5] = 3;
	}
	return addMesh(name, vbuffer, ibuffer, EPT_TRIANGLES, 2, aabbox3df(vector3df(-width, -height, 0), vector3df(width, height, 0)));
}

bool CMeshManager::addCube(const char* name, const vector3df& size, SColor color)
{
	uint32_t vcount = 0;
	uint32_t icount = 0;
	getCubeMeshVICount(vcount, icount);

	IVertexBuffer* vbuffer = g_Engine->getDriver()->createVertexBuffer(EMM_STATIC);
	IIndexBuffer* ibuffer = g_Engine->getDriver()->createIndexBuffer(EMM_STATIC);
	{
		SVertex_PCT* vertices = vbuffer->alloc<SVertex_PCT>(vcount);
		uint16_t* indices = ibuffer->alloc<uint16_t>(icount);
		fillCubeMeshVI(vertices, vcount, indices, icount, size, color);
	}

	return addMesh(name, vbuffer, ibuffer, EPT_TRIANGLES, icount / 3, getCubeMeshAABBox(size));
}

bool CMeshManager::addSphere(const char * name, float radius, uint32_t polyCountX, uint32_t polyCountY, SColor color)
{
	uint32_t vcount = 0;
	uint32_t icount = 0;
	getSphereMeshVICount(vcount, icount, polyCountX, polyCountY);

	IVertexBuffer* vbuffer = g_Engine->getDriver()->createVertexBuffer(EMM_STATIC);
	IIndexBuffer* ibuffer = g_Engine->getDriver()->createIndexBuffer(EMM_STATIC);
	{
		SVertex_PCT* vertices = vbuffer->alloc<SVertex_PCT>(vcount);
		uint16_t* indices = ibuffer->alloc<uint16_t>(icount);
		fillSphereMeshVI(vertices, vcount, indices, icount, radius, polyCountX, polyCountY, color);
	}

	return addMesh(name, vbuffer, ibuffer, EPT_TRIANGLES, icount / 3, getSphereMeshAABBox(radius));
}

bool CMeshManager::addCylinder(const char* name, float radius, float height, uint32_t numSegment, SColor color)
{
	uint32_t vcount = 0;
	uint32_t icount = 0;
	getCylinderMeshVICount(vcount, icount, numSegment);

	IVertexBuffer* vbuffer = g_Engine->getDriver()->createVertexBuffer(EMM_STATIC);
	IIndexBuffer* ibuffer = g_Engine->getDriver()->createIndexBuffer(EMM_STATIC);
	{
		SVertex_PCT* vertices = vbuffer->alloc<SVertex_PCT>(vcount);
		uint16_t* indices = ibuffer->alloc<uint16_t>(icount);
		fillCylinderMeshVI(vertices, vcount, indices, icount, radius, height, numSegment, color);
	}

	return addMesh(name, vbuffer, ibuffer, EPT_TRIANGLES, icount / 3, getCylinderMeshAABBox(radius, height));
}

void CMeshManager::getCubeMeshVICount(uint32_t& vcount, uint32_t& icount)
{
	vcount = 12;
	icount = 36;
}

bool CMeshManager::fillCubeMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount, const vector3df& size, SColor color /*= SColor::White()*/)
{
	uint32_t vLimit, iLimit;
	getCubeMeshVICount(vLimit, iLimit);

	if (vcount < vLimit || icount < iLimit)
		return false;

	const uint16_t u[36] = { 0, 2, 1, 0, 3, 2, 1, 5, 4, 1, 2, 5, 4, 6, 7, 4, 5, 6,
		7, 3, 0, 7, 6, 3, 9, 5, 2, 9, 8, 5, 0, 11, 10, 0, 10, 7 };

	Q_memcpy(indices, sizeof(uint16_t) * 36, u, sizeof(u));

	vertices[0].set(vector3df(0, 0, 0), color, vector2df(0, 1));
	vertices[1].set(vector3df(1, 0, 0), color, vector2df(1, 1));
	vertices[2].set(vector3df(1, 1, 0), color, vector2df(1, 0));
	vertices[3].set(vector3df(0, 1, 0), color, vector2df(0, 0));
	vertices[4].set(vector3df(1, 0, 1), color, vector2df(0, 1));
	vertices[5].set(vector3df(1, 1, 1), color, vector2df(0, 0));
	vertices[6].set(vector3df(0, 1, 1), color, vector2df(1, 0));
	vertices[7].set(vector3df(0, 0, 1), color, vector2df(1, 1));
	vertices[8].set(vector3df(0, 1, 1), color, vector2df(0, 1));
	vertices[9].set(vector3df(0, 1, 0), color, vector2df(1, 1));
	vertices[10].set(vector3df(1, 0, 1), color, vector2df(1, 0));
	vertices[11].set(vector3df(1, 0, 0), color, vector2df(0, 0));

	for (uint32_t i = 0; i < 12; ++i)
	{
		vertices[i].Pos -= vector3df(0.5f, 0.5f, 0.5f);
		vertices[i].Pos = vertices[i].Pos * size;
	}

	return true;
}

void CMeshManager::getSphereMeshVICount(uint32_t& vcount, uint32_t& icount, uint32_t polyCountX, uint32_t polyCountY)
{
	if (polyCountX < 2)
		polyCountX = 2;
	if (polyCountY < 2)
		polyCountY = 2;
	while (polyCountX * polyCountY > 32767) // prevent uint16_t overflow
	{
		polyCountX /= 2;
		polyCountY /= 2;
	}

	const uint32_t polyCountXPitch = polyCountX + 1; // get to same vertex on next level

	vcount = polyCountXPitch * polyCountY + 2;
	icount = (polyCountX * polyCountY) * 6;
}

bool CMeshManager::fillSphereMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount, float radius /*= 5.f*/, uint32_t polyCountX /*= 16*/, uint32_t polyCountY /*= 16*/, SColor color /*= SColor::White()*/)
{
	uint32_t vLimit, iLimit;
	if (polyCountX < 2)
		polyCountX = 2;
	if (polyCountY < 2)
		polyCountY = 2;
	while (polyCountX * polyCountY > 32767) // prevent uint16_t overflow
	{
		polyCountX /= 2;
		polyCountY /= 2;
	}

	const uint32_t polyCountXPitch = polyCountX + 1; // get to same vertex on next level

	vLimit = polyCountXPitch * polyCountY + 2;
	iLimit = (polyCountX * polyCountY) * 6;

	if (vcount < vLimit || icount < iLimit)
		return false;

	//¼ÆÊý
	vcount = 0;
	icount = 0;

	uint32_t level = 0;

	for (uint32_t p1 = 0; p1 < polyCountY - 1; ++p1)
	{
		//main quads, top to bottom
		for (uint32_t p2 = 0; p2 < polyCountX - 1; ++p2)
		{
			uint32_t curr = level + p2;
			indices[icount++] = (uint16_t)(curr + polyCountXPitch);
			indices[icount++] = (uint16_t)(curr);
			indices[icount++] = (uint16_t)(curr + 1);
			indices[icount++] = (uint16_t)(curr + polyCountXPitch);
			indices[icount++] = (uint16_t)(curr + 1);
			indices[icount++] = (uint16_t)(curr + 1 + polyCountXPitch);
		}

		// the connectors from front to end
		indices[icount++] = (uint16_t)(level + polyCountX - 1 + polyCountXPitch);
		indices[icount++] = (uint16_t)(level + polyCountX - 1);
		indices[icount++] = (uint16_t)(level + polyCountX);

		indices[icount++] = (uint16_t)(level + polyCountX - 1 + polyCountXPitch);
		indices[icount++] = (uint16_t)(level + polyCountX);
		indices[icount++] = (uint16_t)(level + polyCountX + polyCountXPitch);
		level += polyCountXPitch;
	}

	const uint32_t polyCountSq = polyCountXPitch * polyCountY; // top point
	const uint32_t polyCountSq1 = polyCountSq + 1; // bottom point
	const uint32_t polyCountSqM1 = (polyCountY - 1) * polyCountXPitch; // last row's first vertex

	for (uint32_t p2 = 0; p2 < polyCountX - 1; ++p2)
	{
		// create triangles which are at the top of the sphere

		indices[icount++] = (uint16_t)(polyCountSq);
		indices[icount++] = (uint16_t)(p2 + 1);
		indices[icount++] = (uint16_t)(p2);

		// create triangles which are at the bottom of the sphere

		indices[icount++] = (uint16_t)(polyCountSqM1 + p2);
		indices[icount++] = (uint16_t)(polyCountSqM1 + p2 + 1);
		indices[icount++] = (uint16_t)(polyCountSq1);
	}

	// create final triangle which is at the top of the sphere
	indices[icount++] = (uint16_t)(polyCountSq);
	indices[icount++] = (uint16_t)(polyCountX);
	indices[icount++] = (uint16_t)(polyCountX - 1);

	// create final triangle which is at the bottom of the sphere
	indices[icount++] = (uint16_t)(polyCountSqM1 + polyCountX - 1);
	indices[icount++] = (uint16_t)(polyCountSqM1);
	indices[icount++] = (uint16_t)(polyCountSq1);

	// calculate the angle which separates all points in a circle
	const float AngleX = 2 * PI / polyCountX;
	const float AngleY = PI / polyCountY;

	float axz;

	// we don't start at 0.

	float ay = 0;//AngleY / 2;

	for (uint32_t y = 0; y < polyCountY; ++y)
	{
		ay += AngleY;
		const float sinay = sin(ay);
		axz = 0;

		// calculate the necessary vertices without the doubled one
		for (uint32_t xz = 0; xz < polyCountX; ++xz)
		{
			// calculate points position

			const vector3df pos(static_cast<float>(radius * cos(axz) * sinay),
				static_cast<float>(radius * cos(ay)),
				static_cast<float>(radius * sin(axz) * sinay));
			// for spheres the normal is the position
			vector3df normal(pos);
			normal.normalize();

			// calculate texture coordinates via sphere mapping
			// tu is the same on each level, so only calculate once
			float tu = 0.5f;
			if (y == 0)
			{
				if (normal.y != -1.0f && normal.y != 1.0f)
					tu = acos(clamp_((float)(normal.x / sinay), -1.0f, 1.0f)) * 0.5f *reciprocal_(PI);
				if (normal.z < 0.0f)
					tu = 1 - tu;
			}
			else
				tu = vertices[vcount - polyCountXPitch].TCoords.x;

			vertices[vcount++].set(vector3df(pos.x, pos.y, pos.z),
				color,
				vector2df(tu, ay*reciprocal_(PI)));

			axz += AngleX;
		}
		// This is the doubled vertex on the initial position
		vertices[vcount] = vertices[vcount - polyCountX];
		vertices[vcount].TCoords.x = 1.0f;
		++vcount;
	}

	// the vertex at the top of the sphere
	vertices[vcount++].set(vector3df(0.0f, radius, 0.0f), color, vector2df(0.5f, 0.0f));

	// the vertex at the bottom of the sphere
	vertices[vcount++].set(vector3df(0.0f, -radius, 0.0f), color, vector2df(0.5f, 1.0f));

	ASSERT(polyCountXPitch * polyCountY + 2 == vcount);
	ASSERT((polyCountX * polyCountY) * 6 == icount);

	return true;
}

void CMeshManager::getCylinderMeshVICount(uint32_t& vcount, uint32_t& icount, uint32_t numSegment)
{
	vcount = (numSegment + 1) * 2;
	icount = numSegment * 12;
}

bool CMeshManager::fillCylinderMeshVI(SVertex_PCT* vertices, uint32_t vcount, uint16_t* indices, uint32_t icount, float radius, float height, uint32_t numSegment /*= 20*/, SColor color /*= SColor()*/)
{
	uint32_t vLimit = (numSegment + 1) * 2;
	uint32_t iLimit = numSegment * 12;

	if (vLimit > vcount || iLimit > icount)
		return false;

	float fPiDelta = 2 * PI / numSegment;

	vertices[vLimit - 2].set(vector3df(0, height*0.5f, 0), color, vector2df(0, 0));
	vertices[vLimit - 1].set(vector3df(0, -height*0.5f, 0), color, vector2df(1, 1));

	float angle = 0;
	int iIndex = 0;
	//0 - pi2
	for (uint32_t i = 0; i < numSegment; ++i)
	{
		vertices[i].Pos.x = radius*cos(angle);
		vertices[i].Pos.z = radius*sin(angle);
		vertices[i].Pos.y = height*0.5f;
		vertices[i].Color = color;
		vertices[i].TCoords.set(i / (float)numSegment, 0.0f);

		vertices[i + numSegment] = vertices[i];
		vertices[i + numSegment].Pos.y = vertices[i + numSegment].Pos.y - height;
		vertices[i + numSegment].Color = color;
		vertices[i].TCoords.set(i / (float)numSegment, 1.0f);

		indices[iIndex++] = i;
		indices[iIndex++] = vLimit - 2;
		indices[iIndex++] = (i + 1) % (numSegment);

		indices[iIndex++] = i + numSegment;
		indices[iIndex++] = (i + 1) % (numSegment)+numSegment;
		indices[iIndex++] = vLimit - 1;

		indices[iIndex++] = i;
		indices[iIndex++] = (i + 1) % (numSegment);
		indices[iIndex++] = (i + 1) % (numSegment)+numSegment;

		indices[iIndex++] = i;
		indices[iIndex++] = (i + 1) % (numSegment)+numSegment;
		indices[iIndex++] = i % (numSegment)+numSegment;

		angle += fPiDelta;
	}

	return true;
}

uint32_t CMeshManager::getGridLineVCount(uint32_t xzCount)
{
	return (2 * xzCount + 1) * 4;
}

bool CMeshManager::fillGridLineMeshV(SVertex_PC* gVertices, uint32_t vcount, uint32_t xzCount, float gridSize, SColor color, SColor centerColor)
{
	uint32_t vLimit = (2 * xzCount + 1) * 4;
	if (vcount < vLimit)
		return false;

	//
	vcount = 0;

	float halfWidth = xzCount * gridSize;

	//x
	gVertices[vcount++].set(vector3df(-halfWidth, 0, 0), centerColor);
	gVertices[vcount++].set(vector3df(halfWidth, 0, 0), centerColor);

	for (uint32_t i = 1; i <= xzCount; ++i)
	{
		float offset = i * gridSize;
		gVertices[vcount++].set(vector3df(-halfWidth, 0, offset), color);
		gVertices[vcount++].set(vector3df(halfWidth, 0, offset), color);
		gVertices[vcount++].set(vector3df(-halfWidth, 0, -offset), color);
		gVertices[vcount++].set(vector3df(halfWidth, 0, -offset), color);
	}

	//z
	gVertices[vcount++].set(vector3df(0, 0, -halfWidth), centerColor);
	gVertices[vcount++].set(vector3df(0, 0, halfWidth), centerColor);

	for (uint32_t i = 1; i <= xzCount; ++i)
	{
		float offset = i * gridSize;
		gVertices[vcount++].set(vector3df(offset, 0, -halfWidth), color);
		gVertices[vcount++].set(vector3df(offset, 0, halfWidth), color);
		gVertices[vcount++].set(vector3df(-offset, 0, -halfWidth), color);
		gVertices[vcount++].set(vector3df(-offset, 0, halfWidth), color);
	}

	ASSERT(vcount == vLimit);

	return true;
}
