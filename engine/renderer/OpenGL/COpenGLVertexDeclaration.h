#pragma once

#include "COpenGLPCH.h"
#include "base.h"
#include <vector>

class COpenGLDriver;
struct SGLProgram;
class IVertexBuffer;


class COpenGLVertexDeclaration
{
public:
	COpenGLVertexDeclaration(COpenGLDriver* driver, E_VERTEX_TYPE vtype);
	~COpenGLVertexDeclaration();

public:
	void apply(const SGLProgram* program, const IVertexBuffer* vbuffer0, uint32_t offset0);
	void unapply();

	void deleteVao(IVertexBuffer* vbuffer0);

private:
	struct SVAOParam
	{
		const SGLProgram* program;
		const IVertexBuffer* vbuffer0;
		uint32_t offset0;

		bool operator<(const SVAOParam& other) const
		{
			if (program != other.program)
				return program < other.program;
			else if (vbuffer0 != other.vbuffer0)
				return vbuffer0 < other.vbuffer0;
			else
				return offset0 < other.offset0;
		}

		bool operator==(const SVAOParam& other) const
		{
			return program == other.program &&
				vbuffer0 == other.vbuffer0 &&
				offset0 == other.offset0;
		}
	};

	struct SElementInfo
	{
		SElementInfo(int32_t _vOffset, GLint _size, GLenum _type, GLboolean _normalized)
			: vOffset(_vOffset), size(_size), type(_type), normalized(_normalized) {
			location = 0;
		}

		int32_t location;
		int32_t vOffset;			//offset in vertex
		GLint size;
		GLenum	type;
		GLboolean	normalized;
	};

	struct SVertexInfo
	{
		uint32_t vertexSize;
		std::vector<SElementInfo>		elementInfos;
	};
};