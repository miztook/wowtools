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

public:
	uint32_t getTextureCount() const { return (uint32_t)textureNameIndexMap.size(); }
	const SGLUniformInfo* getUniform(const char* name) const
	{
		auto itr = uniformNameIndexMap.find(name);
		if (itr == uniformNameIndexMap.end())
			return nullptr;
		int index = itr->second;
		return &uniformList[index];
	}
	int getTextureIndex(const char* name) const
	{
		auto itr = textureNameIndexMap.find(name);
		if (itr == textureNameIndexMap.end())
			return -1;
		return itr->second;
	}

public:
	//uniform info
	std::vector<SGLUniformInfo> uniformList;
	std::map<std::string, int>	uniformNameIndexMap;
	std::map<std::string, int> textureNameIndexMap;
};

class COpenGLVertexShader : public IVideoResource
{
public:
	COpenGLVertexShader(const COpenGLDriver* driver, const char* name, const char* macroString);
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
	std::string Name;
	std::string MacroString;
	bool	VideoBuilt;
};

class COpenGLPixelShader : public IVideoResource
{
public:
	COpenGLPixelShader(const COpenGLDriver* driver, const char* name, const char* macroString);
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
	std::string Name;
	std::string MacroString;
	bool	VideoBuilt;
};

class COpenGLShaderManageComponent
{
public:
	explicit COpenGLShaderManageComponent(const COpenGLDriver* driver);
	~COpenGLShaderManageComponent();

public:
	bool init();

	const CGLProgram* applyShaders(const SMaterial* material, E_VERTEX_TYPE vertexType);
	const COpenGLVertexShader* getVertexShader(const char* fileName, const char* macroString = "");
	const COpenGLPixelShader* getPixelShader(const char* fileName, const char* macroString = "");

	const COpenGLVertexShader* getDefaultVertexShader(E_VERTEX_TYPE vType, const char* macroString);
	const COpenGLPixelShader* getDefaultPixelShader(E_VERTEX_TYPE vType, const char* macroString);

	void addMacroByMaterial(const SMaterial& material, std::set<std::string>& shaderMacro) const;

	const CGLProgram* findGLProgram(const COpenGLVertexShader* vshader, const COpenGLPixelShader* pshader) const;
	const CGLProgram* getGlProgram(const COpenGLVertexShader* vshader, const COpenGLPixelShader* pshader);
	const CGLProgram* createGLProgram(const COpenGLVertexShader* vshader, const COpenGLPixelShader* pshader);

	void setShaderUniformF(const SGLUniformInfo* uniform, const matrix4& mat) { setShaderUniformF(uniform, mat.M, sizeof(matrix4)); }
	void setShaderUniformF(const SGLUniformInfo* uniform, const vector4df& vec) { setShaderUniformF(uniform, &vec.x, sizeof(vector4df)); }
	void setShaderUniformF(const SGLUniformInfo* uniform, const float* srcData, uint32_t size)
	{
		setShaderUniformF(uniform->location, uniform->type, srcData, size);
	}
	void setShaderUniformF(uint32_t location, GLenum type, const float* srcData, uint32_t size);

	const char* getVSDir() const { return VertexShaderDir.c_str(); }
	const char* getPSDir() const { return PixelShaderDir.c_str(); }
	
	//
	void setGlobalVariables(const CGLProgram* program, bool is2D);
	void setMaterialVariables(const CGLProgram* program, const SMaterial* material);

private:
	struct SShaderState
	{
		void reset()
		{
			vshader = nullptr;
			pshader = nullptr;
		}

		const COpenGLVertexShader*		vshader;
		const COpenGLPixelShader*		pshader;
	};

	struct SShaderKey
	{
		std::string fileName;
		std::string macroString;

		bool operator<(const SShaderKey& other) const
		{
			if (fileName != other.fileName)
				return fileName < other.fileName;
			else
				return macroString < other.macroString;
		}
	};

	struct SProgramKey
	{
		const COpenGLVertexShader* vshader;
		const COpenGLPixelShader* pshader;

		bool operator<(const SProgramKey& other) const
		{
			if (vshader != other.vshader)
				return vshader < other.vshader;
			else
				return pshader < other.pshader;
		}
	};

private:
	const COpenGLDriver*	Driver;
	const CGLProgram*		CurrentProgram;
	SShaderState	ShaderState;

	std::string VertexShaderDir;
	std::string PixelShaderDir;

	std::map<SShaderKey, const COpenGLVertexShader*> VertexShaderMap;
	std::map<SShaderKey, const COpenGLPixelShader*> PixelShaderMap;
	std::map<SProgramKey, const CGLProgram*> ProgramMap;
};