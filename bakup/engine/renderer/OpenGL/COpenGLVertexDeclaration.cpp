#include "COpenGLVertexDeclaration.h"

#include "S3DVertex.h"
#include "COpenGLDriver.h"
#include "COpenGLHelper.h"
#include "COpenGLVertexIndexBuffer.h"
#include "COpenGLShaderManageComponent.h"

#define NAME_POS		"Pos"
#define NAME_NORMAL		"Normal"
#define NAME_COLOR0		"Col0"
#define NAME_COLOR1		"Col1"
#define NAME_TEX0				"Tex0"
#define NAME_TEX1				"Tex1"
#define NAME_WEIGHT		"BlendWeight"
#define NAME_BLENDINDICES		"BlendIndices"
#define NAME_BINORMAL		"Binormal"
#define NAME_TANGENT		"Tangent"

#define buffer_offset COpenGLHelper::buffer_offset

COpenGLVertexDeclaration::COpenGLVertexDeclaration(COpenGLDriver* driver, E_VERTEX_TYPE vtype)
	: Driver(driver), VertexType(vtype)
{
}

COpenGLVertexDeclaration::~COpenGLVertexDeclaration()
{
	ASSERT(VaoMap.empty());
	for (const auto& kv : VaoMap)
	{
		Driver->GLExtension.extGlDeleteVertexArrays(1, &kv.second);
	}
	VaoMap.clear();
}

void COpenGLVertexDeclaration::apply(const CGLProgram* program, const IVertexBuffer* vbuffer0, uint32_t offset0)
{
	if (Driver->GLExtension.canUseVAO())
	{
		SVAOParam param;
		param.program = program;
		param.vbuffer0 = vbuffer0;
		param.offset0 = offset0;

		GLuint vao = getVao(param);
		Driver->GLExtension.extGlBindVertexArray(vao);
	}
	else
	{
		const SVertexInfo& vertexInfo = getVertexInfo(program);

		ASSERT(vbuffer0);
		Driver->GLExtension.extGlBindBuffer(GL_ARRAY_BUFFER, static_cast<const COpenGLVertexBuffer*>(vbuffer0)->getHWBuffer());

		applyVbo(vertexInfo, offset0);
	}
}

void COpenGLVertexDeclaration::unapply()
{
	if (Driver->GLExtension.canUseVAO())
	{
		Driver->GLExtension.extGlBindVertexArray(0);
	}
	else
	{
		Driver->GLExtension.extGlBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void COpenGLVertexDeclaration::deleteVao(const IVertexBuffer* vbuffer0)
{
	ASSERT(vbuffer0->getVertexType() == VertexType);
	for (auto itr = VaoMap.begin(); itr != VaoMap.end();)
	{
		if (itr->first.vbuffer0 == vbuffer0)
			VaoMap.erase(itr++);
		else
			++itr;
	}
}

GLuint COpenGLVertexDeclaration::createVao(const SVAOParam& param)
{
	GLuint vao;
	Driver->GLExtension.extGlGenVertexArrays(1, &vao);

	Driver->GLExtension.extGlBindVertexArray(vao);

	bindVbo(param.program, param.vbuffer0, param.offset0);

	Driver->GLExtension.extGlBindVertexArray(0);

	return vao;
}

GLuint COpenGLVertexDeclaration::getVao(const SVAOParam& param)
{
	auto itr = VaoMap.find(param);
	if (itr == VaoMap.end())
	{
		GLuint vao = createVao(param);
		VaoMap[param] = vao;
		return vao;
	}
	return itr->second;
}

COpenGLVertexDeclaration::SVertexInfo COpenGLVertexDeclaration::createVertexInfo(const CGLProgram* program)
{
	SVertexInfo vertexInfo;
	switch (VertexType)
	{
	case EVT_PC:
		vertexInfo = createVertexInfo_PC(program);
		break;
	case EVT_PCT:
		vertexInfo = createVertexInfo_PCT(program);
		break;
	case EVT_PNC:
		vertexInfo = createVertexInfo_PNC(program);
		break;
	case EVT_PNCT:
		vertexInfo = createVertexInfo_PNCT(program);
		break;
	case EVT_PNCT2:
		vertexInfo = createVertexInfo_PNCT2(program);
		break;
	case EVT_PNT2WA:
		vertexInfo = createVertexInfo_PNT2WA(program);
		break;
	default:
		ASSERT(false);
		break;
	}
	return vertexInfo;
}

const COpenGLVertexDeclaration::SVertexInfo& COpenGLVertexDeclaration::getVertexInfo(const CGLProgram * program)
{
	const CGLProgram* param = program;

	auto itr = VInfoMap.find(param);
	if (itr == VInfoMap.end())
	{
		SVertexInfo vInfo = createVertexInfo(param);
		VInfoMap[param] = vInfo;
		return VInfoMap[param];
	}
	return itr->second;
}

void COpenGLVertexDeclaration::bindVbo(const CGLProgram* program, const IVertexBuffer* vbuffer0, uint32_t offset0)
{
	ASSERT(vbuffer0);
	Driver->GLExtension.extGlBindBuffer(GL_ARRAY_BUFFER, static_cast<const COpenGLVertexBuffer*>(vbuffer0)->getHWBuffer());

	const SVertexInfo& vertexInfo = getVertexInfo(program);

	applyVbo(vertexInfo, offset0);

	Driver->GLExtension.extGlBindBuffer(GL_ARRAY_BUFFER, 0);
}

COpenGLVertexDeclaration::SVertexInfo COpenGLVertexDeclaration::createVertexInfo_PC(const CGLProgram * program)
{
	SVertexInfo vertexInfo;
	vertexInfo.vertexSize = sizeof(SVertex_PC);

	//position
	{
		SElementInfo element(0, 3, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_POS);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//color0
	{
		SElementInfo element(12, 4, GL_UNSIGNED_BYTE, GL_TRUE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_COLOR0);
		vertexInfo.elementInfos.emplace_back(element);
	}

	return vertexInfo;
}

COpenGLVertexDeclaration::SVertexInfo COpenGLVertexDeclaration::createVertexInfo_PCT(const CGLProgram * program)
{
	SVertexInfo vertexInfo;
	vertexInfo.vertexSize = sizeof(SVertex_PCT);

	//position
	{
		SElementInfo element(0, 3, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_POS);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//color0
	{
		SElementInfo element(12, 4, GL_UNSIGNED_BYTE, GL_TRUE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_COLOR0);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//tex0
	{
		SElementInfo element(16, 2, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_TEX0);
		vertexInfo.elementInfos.emplace_back(element);
	}

	return vertexInfo;
}

COpenGLVertexDeclaration::SVertexInfo COpenGLVertexDeclaration::createVertexInfo_PNC(const CGLProgram * program)
{
	SVertexInfo vertexInfo;
	vertexInfo.vertexSize = sizeof(SVertex_PNC);

	//position
	{
		SElementInfo element(0, 3, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_POS);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//normal
	{
		SElementInfo element(12, 3, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_NORMAL);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//color
	{
		SElementInfo element(24, 4, GL_UNSIGNED_BYTE, GL_TRUE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_COLOR0);
		vertexInfo.elementInfos.emplace_back(element);
	}

	return vertexInfo;
}

COpenGLVertexDeclaration::SVertexInfo COpenGLVertexDeclaration::createVertexInfo_PNCT(const CGLProgram* program)
{
	SVertexInfo vertexInfo;
	vertexInfo.vertexSize = sizeof(SVertex_PNCT);

	//position
	{
		SElementInfo element(0, 3, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_POS);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//normal
	{
		SElementInfo element(12, 3, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_NORMAL);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//color
	{
		SElementInfo element(24, 4, GL_UNSIGNED_BYTE, GL_TRUE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_COLOR0);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//tex0
	{
		SElementInfo element(28, 2, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_TEX0);
		vertexInfo.elementInfos.emplace_back(element);
	}

	return vertexInfo;
}

COpenGLVertexDeclaration::SVertexInfo COpenGLVertexDeclaration::createVertexInfo_PNCT2(const CGLProgram * program)
{
	SVertexInfo vertexInfo;
	vertexInfo.vertexSize = sizeof(SVertex_PNCT2);

	//position
	{
		SElementInfo element(0, 3, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_POS);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//normal
	{
		SElementInfo element(12, 3, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_NORMAL);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//color
	{
		SElementInfo element(24, 4, GL_UNSIGNED_BYTE, GL_TRUE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_COLOR0);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//tex0
	{
		SElementInfo element(28, 2, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_TEX0);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//tex1
	{
		SElementInfo element(36, 2, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_TEX1);
		vertexInfo.elementInfos.emplace_back(element);
	}

	return vertexInfo;
}

COpenGLVertexDeclaration::SVertexInfo COpenGLVertexDeclaration::createVertexInfo_PNT2WA(const CGLProgram* program)
{
	SVertexInfo vertexInfo;
	vertexInfo.vertexSize = sizeof(SVertex_PNT2WA);

	//position
	{
		SElementInfo element(0, 3, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_POS);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//normal
	{
		SElementInfo element(12, 3, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_NORMAL);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//tex0
	{
		SElementInfo element(24, 2, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_TEX0);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//tex1
	{
		SElementInfo element(32, 2, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_TEX1);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//weight
	{
		SElementInfo element(40, 3, GL_FLOAT, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_WEIGHT);
		vertexInfo.elementInfos.emplace_back(element);
	}

	//blendindices
	{
		SElementInfo element(52, 4, GL_UNSIGNED_BYTE, GL_FALSE);
		element.location = (int32_t)Driver->GLExtension.extGlGetAttribLocationARB(program->handle, NAME_BLENDINDICES);
		vertexInfo.elementInfos.emplace_back(element);
	}

	return vertexInfo;
}

void COpenGLVertexDeclaration::applyVbo(const SVertexInfo & vertexInfo, uint32_t offset0)
{
	for (const auto& elementInfo : vertexInfo.elementInfos)
	{
		int32_t index = elementInfo.location;
		if (index >= 0)
		{
			Driver->GLExtension.extGlEnableVertexAttribArrayARB(index);
			Driver->GLExtension.extGlVertexAttribPointerARB(index, elementInfo.size, elementInfo.type, elementInfo.normalized, vertexInfo.vertexSize, buffer_offset(elementInfo.vOffset + vertexInfo.vertexSize * offset0));
		}
	}
}
