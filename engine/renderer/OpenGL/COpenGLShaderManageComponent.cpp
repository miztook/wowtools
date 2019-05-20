#include "COpenGLShaderManageComponent.h"

#include "COpenGLPCH.h"
#include "COpenGLDriver.h"
#include "CShaderUtil.h"
#include "CFileSystem.h"

COpenGLShaderManageComponent::COpenGLShaderManageComponent(const COpenGLDriver* driver)
	: Driver(driver), CurrentProgram(nullptr)
{
	VertexShaderDir = CShaderUtil::getVSDir("glvs_15");
	PixelShaderDir = CShaderUtil::getPSDir("glps_15");
}

COpenGLShaderManageComponent::~COpenGLShaderManageComponent()
{
	Driver->GLExtension.extGlUseProgramObject(0);

	for (const auto& itr : ProgramMap)
	{
		const CGLProgram* program = itr.second;
		Driver->GLExtension.extGlDeleteObject(program->handle);
		delete program;
	}
}

const COpenGLVertexShader* COpenGLShaderManageComponent::getVertexShader(const char* fileName, const char* macroString)
{
	SShaderKey key;
	key.fileName = fileName;
	key.macroString = macroString;

	auto itr = VertexShaderMap.find(key);
	if (itr != VertexShaderMap.end())
	{
		return itr->second;
	}

	COpenGLVertexShader* vshader = new COpenGLVertexShader(Driver, fileName, macroString);
	if (!vshader->compile())
	{
		delete vshader;
		return nullptr;
	}

	VertexShaderMap[key] = vshader;

	return vshader;
}

const COpenGLPixelShader* COpenGLShaderManageComponent::getPixelShader(const char* fileName, const char* macroString)
{
	SShaderKey key;
	key.fileName = fileName;
	key.macroString = macroString;

	auto itr = PixelShaderMap.find(key);
	if (itr != PixelShaderMap.end())
	{
		return itr->second;
	}

	COpenGLPixelShader* pshader = new COpenGLPixelShader(Driver, fileName, macroString);
	if (!pshader->compile())
	{
		delete pshader;
		return nullptr;
	}

	PixelShaderMap[key] = pshader;

	return pshader;
}

void COpenGLShaderManageComponent::addMacroByMaterial(const SMaterial& material, std::set<std::string>& shaderMacro) const
{
	if (material.isAlphaTest())
	{
		shaderMacro.insert("ALPHA_TEST");
	}
}

const CGLProgram* COpenGLShaderManageComponent::findGLProgram(const COpenGLVertexShader* vshader, const COpenGLPixelShader* pshader) const
{
	SProgramKey key;
	key.vshader = vshader;
	key.pshader = pshader;

	ASSERT(vshader && pshader);
	auto itr = ProgramMap.find(key);
	if (itr != ProgramMap.end())
		return itr->second;
	return nullptr;
}

const CGLProgram* COpenGLShaderManageComponent::getGlProgram(const COpenGLVertexShader* vshader, const COpenGLPixelShader* pshader)
{
	const CGLProgram* program = findGLProgram(vshader, pshader);
	if (!program)
	{
		SProgramKey key;
		key.vshader = vshader;
		key.pshader = pshader;

		program = createGLProgram(vshader, pshader);
		ProgramMap[key] = program;
	}

	return program;
}

const CGLProgram* COpenGLShaderManageComponent::createGLProgram(const COpenGLVertexShader* vshader, const COpenGLPixelShader* pshader)
{
	if (!vshader || !pshader)
	{
		ASSERT(false);
		return nullptr;
	}

	GLhandleARB p = Driver->GLExtension.extGlCreateProgramObject();

	Driver->GLExtension.extGlAttachObject(p, (GLhandleARB)vshader->getGLShader());
	Driver->GLExtension.extGlAttachObject(p, (GLhandleARB)pshader->getGLShader());

	//bind ps output
	Driver->GLExtension.extGlBindFragDataLocation(p, 0, "COLOR0");
	Driver->GLExtension.extGlBindFragDataLocation(p, 1, "COLOR1");
	Driver->GLExtension.extGlBindFragDataLocation(p, 2, "COLOR2");
	Driver->GLExtension.extGlBindFragDataLocation(p, 3, "COLOR3");

	//link
	Driver->GLExtension.extGlLinkProgramARB(p);

	GLint status = 0;
	Driver->GLExtension.extGlGetObjectParameteriv(p, GL_OBJECT_LINK_STATUS_ARB, &status);

	if (!status)
	{
		g_FileSystem->writeLog(ELOG_GX, "GLSL shader program failed to link: ");
		// check error message and log it
		GLint maxLength = 0;
		GLsizei length;

		Driver->GLExtension.extGlGetObjectParameteriv(p, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);

		GLcharARB *infoLog = new GLcharARB[maxLength];
		Driver->GLExtension.extGlGetInfoLog(p, maxLength, &length, infoLog);
		g_FileSystem->writeLog(ELOG_GX, "GLSL shader program failed to link: %s", reinterpret_cast<const char*>(infoLog));
		delete[] infoLog;

		//remove
		Driver->GLExtension.extGlDeleteObject(p);
		return nullptr;
	}

	CGLProgram* program = new CGLProgram(p);

	//get uniform info
	Driver->GLExtension.extGlUseProgramObject(p);

	GLint numUniforms = 0;
	Driver->GLExtension.extGlGetObjectParameteriv(p, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &numUniforms);
	GLint maxLen = 0;
	Driver->GLExtension.extGlGetObjectParameteriv(p, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &maxLen);

	//
	program->uniformList.resize(numUniforms);
	std::vector<char> buf;
	buf.resize(maxLen);
	for (int i = 0; i < (int)numUniforms; ++i)
	{
		SGLUniformInfo info;

		GLenum type;
		GLint length;
		GLint size;
		Driver->GLExtension.extGlGetActiveUniformARB(p, i, maxLen, &length, &size, &type, (GLcharARB*)buf.data());
		buf[length] = '\0';
		std::string name = buf.data();
		info.dimension = (uint8_t)size;
		info.type = type;
		info.location = Driver->GLExtension.extGlGetUniformLocationARB(p, name.c_str());

		//delete "[]" in array names
		std::size_t offset = name.find('[', 0);
		if (offset != std::string::npos)
		{
			name = name.substr(0, offset);
		}

		ASSERT(type != GL_SAMPLER_1D_ARB && type != GL_SAMPLER_3D_ARB);

		program->uniformList.emplace_back(info);
		program->uniformNameIndexMap[name] = (uint32_t)program->uniformList.size() - 1;
		
		if (info.isTexture())
		{
			info.textureIndex = (int)program->textureNameIndexMap.size();
			program->textureNameIndexMap[name] = (uint32_t)program->uniformList.size() - 1;
			//
			Driver->GLExtension.extGlUniform1iv(info.location, info.type, &(GLint)info.textureIndex);
		}
		else
		{
			info.textureIndex = -1;
		}
	}

	Driver->GLExtension.extGlUseProgramObject(0);

	return program;
}

void COpenGLShaderManageComponent::setShaderUniformF(uint32_t location, GLenum type, const float* srcData, uint32_t size)
{
	uint32_t count = size / sizeof(float);

	switch (type)
	{
	case GL_FLOAT:
		Driver->GLExtension.extGlUniform1fv(location, count, srcData);
		break;
	case GL_FLOAT_VEC2_ARB:
		Driver->GLExtension.extGlUniform2fv(location, count / 2, srcData);
		break;
	case GL_FLOAT_VEC3_ARB:
		Driver->GLExtension.extGlUniform3fv(location, count / 3, srcData);
		break;
	case GL_FLOAT_VEC4_ARB:
		Driver->GLExtension.extGlUniform4fv(location, count / 4, srcData);
		break;
	case GL_FLOAT_MAT2_ARB:
		Driver->GLExtension.extGlUniformMatrix2fv(location, count / 4, GL_FALSE, srcData);
		break;
	case GL_FLOAT_MAT3_ARB:
		Driver->GLExtension.extGlUniformMatrix3fv(location, count / 9, GL_FALSE, srcData);
		break;
	case GL_FLOAT_MAT4_ARB:
		Driver->GLExtension.extGlUniformMatrix4fv(location, count / 16, GL_FALSE, srcData);
		break;
	default:
		ASSERT(false);
		Driver->GLExtension.extGlUniform4fv(location, count / 4, srcData);
		break;
	}
}

COpenGLVertexShader::COpenGLVertexShader(const COpenGLDriver* driver, const char* name, const char* macroString)
	: Driver(driver), Name(name), MacroString(macroString)
{
}

COpenGLVertexShader::~COpenGLVertexShader()
{
	releaseVideoResources();
}

bool COpenGLVertexShader::compile()
{
	return buildVideoResources();
}

bool COpenGLVertexShader::buildVideoResources()
{
	if (VideoBuilt)
		return false;

	std::string absFileName = Driver->getShaderManageComponent()->getVSDir();
	absFileName.append(Name);
	absFileName.append(".glsl");

	SShaderFile result;
	if (!CShaderUtil::loadFile_OpenGL(absFileName.c_str(), CShaderUtil::getShaderMacroSet(MacroString.c_str()), result))
		return false;

	const char* buffer = result.Buffer.data();

	GLhandleARB shaderHandle = Driver->GLExtension.extGlCreateShaderObject(GL_VERTEX_SHADER_ARB);
	Driver->GLExtension.extGlShaderSourceARB(shaderHandle, 1, &buffer, nullptr);
	Driver->GLExtension.extGlCompileShaderARB(shaderHandle);

	GLint status = 0;

	Driver->GLExtension.extGlGetObjectParameteriv(shaderHandle, GL_OBJECT_COMPILE_STATUS_ARB, &status);

	if (!status)
	{
		g_FileSystem->writeLog(ELOG_GX, "GLSL shader failed to compile: %s, %s", absFileName.c_str(), MacroString.c_str());
		// check error message and log it
		GLint maxLength = 0;
		GLsizei length;

		Driver->GLExtension.extGlGetObjectParameteriv(shaderHandle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);

		GLcharARB* infoLog = new GLcharARB[maxLength];
		Driver->GLExtension.extGlGetInfoLog(shaderHandle, maxLength, &length, infoLog);
		g_FileSystem->writeLog(ELOG_GX, reinterpret_cast<const char*>(infoLog));
		delete[] infoLog;

		shaderHandle = 0;
		return false;
	}

	VertexShader = shaderHandle;
	VideoBuilt = true;
	return true;
}

void COpenGLVertexShader::releaseVideoResources()
{
	if (!VideoBuilt)
		return;

	Driver->GLExtension.extGlDeleteObject((GLhandleARB)VertexShader);

	VideoBuilt = false;
}

COpenGLPixelShader::COpenGLPixelShader(const COpenGLDriver* driver, const char* name, const char* macroString)
	: Driver(driver), Name(name), MacroString(macroString)
{
}

COpenGLPixelShader::~COpenGLPixelShader()
{
	releaseVideoResources();
}

bool COpenGLPixelShader::compile()
{
	return buildVideoResources();
}

bool COpenGLPixelShader::buildVideoResources()
{
	if (VideoBuilt)
		return false;

	std::string absFileName = Driver->getShaderManageComponent()->getPSDir();
	absFileName.append(Name);
	absFileName.append(".glsl");

	SShaderFile result;
	if (!CShaderUtil::loadFile_OpenGL(absFileName.c_str(), CShaderUtil::getShaderMacroSet(MacroString.c_str()), result))
		return false;

	const char* buffer = result.Buffer.data();

	GLhandleARB shaderHandle = Driver->GLExtension.extGlCreateShaderObject(GL_VERTEX_SHADER_ARB);
	Driver->GLExtension.extGlShaderSourceARB(shaderHandle, 1, &buffer, nullptr);
	Driver->GLExtension.extGlCompileShaderARB(shaderHandle);

	GLint status = 0;

	Driver->GLExtension.extGlGetObjectParameteriv(shaderHandle, GL_OBJECT_COMPILE_STATUS_ARB, &status);

	if (!status)
	{
		g_FileSystem->writeLog(ELOG_GX, "GLSL shader failed to compile: %s, %s", absFileName.c_str(), MacroString.c_str());

		// check error message and log it
		GLint maxLength = 0;
		GLsizei length;

		Driver->GLExtension.extGlGetObjectParameteriv(shaderHandle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);

		GLcharARB* infoLog = new GLcharARB[maxLength];
		Driver->GLExtension.extGlGetInfoLog(shaderHandle, maxLength, &length, infoLog);
		g_FileSystem->writeLog(ELOG_GX, reinterpret_cast<const char*>(infoLog));
		delete[] infoLog;

		shaderHandle = 0;
		return false;
	}

	PixelShader = shaderHandle;
	VideoBuilt = true;
	return true;
}

void COpenGLPixelShader::releaseVideoResources()
{
	if (!VideoBuilt)
		return;

	Driver->GLExtension.extGlDeleteObject((GLhandleARB)PixelShader);

	VideoBuilt = false;
}
