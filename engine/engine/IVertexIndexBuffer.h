#pragma once

#include "base.h"
#include "S3DVertex.h"
#include "IVideoResource.h"

class IVertexBuffer : public IVideoResource
{
public:
	explicit IVertexBuffer(E_MESHBUFFER_MAPPING mapping) : Mapping(mapping)
	{
		Vertices = nullptr;
		Type = EVT_INVALID;
		Size = 0;
	}
	virtual ~IVertexBuffer()
	{
		deleteVerticesFromType(Type, Vertices);
	}

public:
	template <class T>
	void updateBuffer(const T* data, uint32_t nVerts)
	{
		ASSERT(getVertexType() == T::TYPE());
		do_updateBuffer(data, nVerts);
	}

	template <class T>
	T* alloc(uint32_t size)
	{
		Vertices = new T[size];
		Type = T::TYPE();
		Size = size;
		return static_cast<T*>(Vertices);
	}

	template <class T>
	T* getVertices() const
	{
		ASSERT(Type == T::TYPE());
		return static_cast<T*>(Vertices);
	}

protected:
	virtual void do_updateBuffer(const void* data, uint32_t nVerts) = 0;

public:
	E_VERTEX_TYPE getVertexType() const { return Type; }
	uint32_t getNumVertices() const { return Size; }
	E_MESHBUFFER_MAPPING getBufferMapping() const { return Mapping; }
	void* getVertices() const { return Vertices; }

protected:
	void*	Vertices;
	uint32_t  Size;
	E_VERTEX_TYPE		Type;
	const E_MESHBUFFER_MAPPING		Mapping;
};

class IIndexBuffer : public IVideoResource
{
public:
	explicit IIndexBuffer(E_MESHBUFFER_MAPPING mapping) : Mapping(mapping) 
	{
		Indices = nullptr;
		Type = EIT_16BIT;
		Size = 0;
	}
	virtual ~IIndexBuffer()
	{
		deleteIndicesFromType(Type, Indices);
	}

public:
	template <class T>
	void updateBuffer(const T* data, uint32_t nIndices)
	{
		ASSERT(getIndexType() == EIT_16BIT ? sizeof(T) == 2 : sizeof(T) == 4);
		do_updateBuffer(data, nIndices);
	}

	template <class T>
	T* alloc(uint32_t size)
	{
		if (sizeof(T) == 2)
			Type = EIT_16BIT;
		else
			Type = EIT_32BIT;

		Indices = new T[size];
		Size = size;
		return static_cast<T*>(Indices);
	}

	template <class T>
	T* getIndices() const
	{
		ASSERT(sizeof(T) == 2 ? Type == EIT_16BIT : Type == EIT_32BIT);
		return static_cast<T*>(Indices);
	}

protected:
	virtual void do_updateBuffer(const void* data, uint32_t nIndices) = 0;

public:
	E_INDEX_TYPE getIndexType() const { return Type; }
	uint32_t getNumIndices() const { return Size; }
	E_MESHBUFFER_MAPPING getBufferMapping() const { return Mapping; }
	void* getIndices() const { return Indices; }

protected:
	void*	Indices;
	const E_MESHBUFFER_MAPPING		Mapping;
	uint32_t  Size;
	E_INDEX_TYPE		Type;
};