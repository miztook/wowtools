#pragma once

#include "COpenGLPCH.h"
#include "base.h"
#include <vector>
#include <map>

class COpenGLDriver;

struct SGLUniformInfo
{
	GLenum type;
	int location;
	int dimension;
};

struct SGLProgram
{
	SGLProgram() : program(0), key(0), samplerCount(0) {}

	GLhandleARB		program;
	uint32_t key;
	uint32_t samplerCount;

	//uniform info
	std::vector<SGLUniformInfo>	uniforms;
	std::map<std::string, uint32_t>	uniformMap;
};

class COpenGLShaderManageComponent
{
public:
	explicit COpenGLShaderManageComponent(const COpenGLDriver* driver);


};