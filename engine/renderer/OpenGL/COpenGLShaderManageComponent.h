#pragma once

#include "COpenGLPCH.h"
#include "base.h"
#include "IVideoResource.h"
#include "CMaterial.h"
#include "SColor.h"
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

class COpenGLShader : public IVideoResource
{
public:
	COpenGLShader(const COpenGLDriver* driver, const char* name, const char* macroString);
	virtual ~COpenGLShader();

public:
	bool isValid() const { return GLShader != 0; }
	GLuint getGLShader() const { return GLShader; }

protected:
	bool buildVideoResources(const char* dir, GLenum shaderType);
	void releaseVideoResources() override;
	bool hasVideoBuilt() const override { return VideoBuilt; }

protected:
	const COpenGLDriver* Driver;
	GLuint	GLShader;
	std::string Name;
	std::string MacroString;
	bool	VideoBuilt;
};

class COpenGLVertexShader : public COpenGLShader
{
public:
	COpenGLVertexShader(const COpenGLDriver* driver, const char* name, const char* macroString)
		: COpenGLShader(driver, name, macroString) {}

public:
	bool compile();

protected:
	bool buildVideoResources() override;
};

class COpenGLPixelShader : public COpenGLShader
{
public:
	COpenGLPixelShader(const COpenGLDriver* driver, const char* name, const char* macroString)
		: COpenGLShader(driver, name, macroString) {}

public:
	bool compile();

protected:
	bool buildVideoResources() override;
};

class COpenGLShaderManageComponent
{
public:
	explicit COpenGLShaderManageComponent(const COpenGLDriver* driver);
	~COpenGLShaderManageComponent();

public:
	bool init();

	const CGLProgram* applyShaders(const CPass* pass, E_VERTEX_TYPE vertexType);
	const COpenGLVertexShader* getVertexShader(const char* fileName, const char* macroString = "");
	const COpenGLPixelShader* getPixelShader(const char* fileName, const char* macroString = "");

	void addMacroByMaterial(const CMaterial& material, std::set<std::string>& shaderMacro) const;

	const CGLProgram* findGLProgram(const COpenGLVertexShader* vshader, const COpenGLPixelShader* pshader) const;
	const CGLProgram* getGlProgram(const COpenGLVertexShader* vshader, const COpenGLPixelShader* pshader);
	const CGLProgram* createGLProgram(const COpenGLVertexShader* vshader, const COpenGLPixelShader* pshader);

	void setShaderUniformF(const SGLUniformInfo* uniform, const matrix4& mat) { setShaderUniformF(uniform, mat.M, sizeof(matrix4)); }
	void setShaderUniformF(const SGLUniformInfo* uniform, const vector4df& vec) { setShaderUniformF(uniform, (const float*)&vec, sizeof(vector4df)); }
	void setShaderUniformF(const SGLUniformInfo* uniform, const SColorf& color) { setShaderUniformF(uniform, (const float*)&color, sizeof(SColorf)); }
	void setShaderUniformF(const SGLUniformInfo* uniform, const float* srcData, uint32_t size)
	{
		setShaderUniformF(uniform->location, uniform->type, srcData, size);
	}
	void setShaderUniformF(uint32_t location, GLenum type, const float* srcData, uint32_t size);

	const char* getVSDir() const { return VertexShaderDir.c_str(); }
	const char* getPSDir() const { return PixelShaderDir.c_str(); }
	
public:
	void setShaderVariables(const CGLProgram* program, const CMaterial* material);

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

	std::string		VertexShaderDir;
	std::string		PixelShaderDir;

	std::map<SShaderKey, const COpenGLVertexShader*> VertexShaderMap;
	std::map<SShaderKey, const COpenGLPixelShader*> PixelShaderMap;
	std::map<SProgramKey, const CGLProgram*> ProgramMap;

};