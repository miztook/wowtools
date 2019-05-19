#pragma once

#include "COpenGLPCH.h"
#include "base.h"
#include "IVideoResource.h"
#include "SMaterial.h"
#include <vector>
#include <map>
#include <set>

class COpenGLDriver;

struct SGLUniformInfo
{
	GLenum type;
	int location;
	int dimension;
	int textureIndex;

	bool isTexture() const {
		return type == GL_SAMPLER_2D_ARB || type == GL_SAMPLER_CUBE_ARB;
	}
};

class CGLProgram
{
public:
	explicit CGLProgram(GLhandleARB h) : handle(h) {}

	GLhandleARB		handle;

	uint32_t getTextureCount() const { return (uint32_t)textureNameIndexMap.size(); }

public:
	//uniform info
	std::vector<SGLUniformInfo> uniformList;
	std::map<std::string, uint32_t>	uniformNameIndexMap;
	std::map<std::string, uint32_t> textureNameIndexMap;
};

class COpenGLVertexShader : public IVideoResource
{
public:
	COpenGLVertexShader(const COpenGLDriver* driver, const char* filename, const std::set<std::string>& macroSet);
	~COpenGLVertexShader();

public:
	bool isValid() const { return VertexShader != 0; }
	GLuint getGLShader() const { return VertexShader; }

	bool compile();

protected:
	bool buildVideoResources() override;
	void releaseVideoResources() override;
	bool hasVideoBuilt() const override { return VideoBuilt; }

private:
	const COpenGLDriver* Driver;
	GLuint	VertexShader;
	std::string AbsFileName;
	std::string Name;
	std::set<std::string> MacroSet;
	bool	VideoBuilt;
};

class COpenGLPixelShader : public IVideoResource
{
public:
	COpenGLPixelShader(const COpenGLDriver* driver, const char* filename, const std::set<std::string>& macroSet);
	~COpenGLPixelShader();

public:
	bool isValid() const { return PixelShader != 0; }
	GLuint getGLShader() const { return PixelShader; }

	bool compile();

protected:
	bool buildVideoResources() override;
	void releaseVideoResources() override;
	bool hasVideoBuilt() const override { return VideoBuilt; }

private:
	const COpenGLDriver* Driver;
	GLuint	PixelShader;
	std::string AbsFileName;
	std::string Name;
	std::set<std::string> MacroSet;
	bool	VideoBuilt;
};

class COpenGLShaderManageComponent
{
public:
	explicit COpenGLShaderManageComponent(const COpenGLDriver* driver);
	~COpenGLShaderManageComponent();

public:
	COpenGLVertexShader* getVertexShader(const char* fileName, const std::set<std::string>& shaderMacro);
	COpenGLPixelShader* getPixelShader(const char* fileName, const std::set<std::string>& shaderMacro);

	COpenGLVertexShader* getDefaultVertexShader(E_VERTEX_TYPE vType, const std::set<std::string>& shaderMacro);
	COpenGLPixelShader* getDefaultPixelShader(E_VERTEX_TYPE vType, const std::set<std::string>& shaderMacro);

	void addMacroByMaterial(const SMaterial& material, std::set<std::string>& shaderMacro) const;

	CGLProgram* findGLProgram(const COpenGLVertexShader* vshader, const COpenGLPixelShader* pshader) const;
	CGLProgram* getGlProgram(const COpenGLVertexShader* vshader, const COpenGLPixelShader* pshader);
	CGLProgram* createGLProgram(const COpenGLVertexShader* vshader, const COpenGLPixelShader* pshader);

	void setShaderUniformF(uint32_t location, GLenum type, const float* srcData, uint32_t size);

private:
	const COpenGLDriver*	Driver;
	const CGLProgram*		CurrentProgram;

	std::map<std::string, std::map<std::string, COpenGLVertexShader*>> VertexShaderMap;
	std::map<std::string, std::map<std::string, COpenGLPixelShader*>> PixelShaderMap;
	std::map<const COpenGLVertexShader*, std::map<const COpenGLPixelShader*, CGLProgram*>> ProgramMap;
};