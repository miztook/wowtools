#include "COpenGLShaderManageComponent.h"

#include "COpenGLPCH.h"
#include "COpenGLDriver.h"
#include "CShaderUtil.h"
#include "CFileSystem.h"
#include "stringext.h"

COpenGLShaderManageComponent::COpenGLShaderManageComponent(const COpenGLDriver* driver)
	: Driver(driver), CurrentProgram(nullptr)
{
	VertexShaderDir = g_FileSystem->getDataDirectory();
	VertexShaderDir.append("Shaders/glvs_15/");
	PixelShaderDir = g_FileSystem->getDataDirectory();
	PixelShaderDir.append("Shaders/glps_15/");

	ShaderState.reset();
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

	for (const auto& itr : VertexShaderMap)
	{
		const COpenGLVertexShader* vs = itr.second;
		delete vs;
	}

	for (const auto& itr : PixelShaderMap)
	{
		const COpenGLPixelShader* ps = itr.second;
		delete ps;
	}
}

bool COpenGLShaderManageComponent::init()
{
	return true;
}

const CGLProgram* COpenGLShaderManageComponent::applyShaders(const SMaterial* material, E_VERTEX_TYPE vertexType)
{
	auto shaderKey = CShaderUtil::getShaderKey(
		material->VSFile.c_str(), material->PSFile.c_str(), material->MacroString.c_str(), vertexType);

	const COpenGLVertexShader* vertexShader = getVertexShader(shaderKey.VSFile.c_str(), shaderKey.MacroString.c_str());
	const COpenGLPixelShader* pixelShader = getPixelShader(shaderKey.PSFile.c_str(), shaderKey.MacroString.c_str());

	if (ShaderState.vshader != vertexShader || ShaderState.pshader != pixelShader)
	{
		if (vertexShader && pixelShader)
		{
			CurrentProgram = getGlProgram(vertexShader, pixelShader);
			Driver->GLExtension.extGlUseProgramObject(CurrentProgram->handle);
		}
		else
		{
			ASSERT(false);
			CurrentProgram = nullptr;
			Driver->GLExtension.extGlUseProgramObject(0);
		}

		ShaderState.vshader = vertexShader;
		ShaderState.pshader = pixelShader;
	}

	return CurrentProgram;
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
		ASSERT(false);
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
		ASSERT(false);
		delete pshader;
		return nullptr;
	}

	PixelShaderMap[key] = pshader;

	return pshader;
}

void COpenGLShaderManageComponent::addMacroByMaterial(const SMaterial& material, std::set<std::string>& shaderMacro) const
{
	if (IsRenderQueueAlphaTest(material.RenderQueue))
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

		if (!program)
		{
			ASSERT(false);
			return nullptr;
		}
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
	Driver->GLExtension.extGlBindFragDataLocation(p, 0, "SV_Target0");
	Driver->GLExtension.extGlBindFragDataLocation(p, 1, "SV_Target1");
	Driver->GLExtension.extGlBindFragDataLocation(p, 2, "SV_Target2");
	Driver->GLExtension.extGlBindFragDataLocation(p, 3, "SV_Target3");

	//link
	Driver->GLExtension.extGlLinkProgramARB(p);

	GLint status = 0;
	Driver->GLExtension.extGlGetObjectParameteriv(p, GL_OBJECT_LINK_STATUS_ARB, &status);

	if (!status)
	{
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
		
		if (info.isTexture())
		{
			info.textureIndex = (int)program->textureNameIndexMap.size();
			program->textureNameIndexMap[name] = info.textureIndex;
			//
			GLint i = (GLint)info.textureIndex;
			Driver->GLExtension.extGlUniform1iv(info.location, 1, &i);
		}
		else
		{
			info.textureIndex = -1;
		}

		program->uniformList.emplace_back(info);
		program->uniformNameIndexMap[name] = (int)program->uniformList.size() - 1;
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
		Driver->GLExtension.extGlUniformMatrix2fv(location, count / 4, GL_TRUE, srcData);
		break;
	case GL_FLOAT_MAT3_ARB:
		Driver->GLExtension.extGlUniformMatrix3fv(location, count / 9, GL_TRUE, srcData);
		break;
	case GL_FLOAT_MAT4_ARB:
		Driver->GLExtension.extGlUniformMatrix4fv(location, count / 16, GL_TRUE, srcData);
		break;
	default:
		ASSERT(false);
		Driver->GLExtension.extGlUniform4fv(location, count / 4, srcData);
		break;
	}
}

void COpenGLShaderManageComponent::setGlobalVariables(const CGLProgram* program, bool is2D)
{
	if (is2D)
	{
		auto uf0 = program->getUniform("g_ObjectToWorld");
		if (uf0)
			setShaderUniformF(uf0, matrix4::Identity());

		auto uf1 = program->getUniform("g_MatrixVP");
		if (uf1)
			setShaderUniformF(uf1, Driver->M_VP2D);
	}
	else
	{
		auto uf0 = program->getUniform("g_ObjectToWorld");
		if (uf0)
			setShaderUniformF(uf0, Driver->M_W);

		auto uf1 = program->getUniform("g_MatrixVP");
		if (uf1)
			setShaderUniformF(uf1, Driver->M_VP);

		auto uf2 = program->getUniform("g_MatrixV");
		if (uf2)
			setShaderUniformF(uf2, Driver->M_V);

		auto uf3 = program->getUniform("g_MatrixP");
		if (uf3)
			setShaderUniformF(uf3, Driver->M_P);
	}
}

void COpenGLShaderManageComponent::setMaterialVariables(const CGLProgram* program, const SMaterial* material)
{
	for (auto kv : material->ShaderVariableMap)
	{
		const char* name = kv.first.c_str();
		const SGLUniformInfo* uniform = program->getUniform(name);
		if (uniform)
		{
			const std::vector<float>& v = kv.second;
			setShaderUniformF(uniform, v.data(), (uint32_t)v.size());
		}		
	}
}

bool COpenGLVertexShader::compile()
{
	return buildVideoResources();
}

bool COpenGLVertexShader::buildVideoResources()
{
	return COpenGLShader::buildVideoResources(Driver->getShaderManageComponent()->getVSDir(), GL_VERTEX_SHADER_ARB);
}

bool COpenGLPixelShader::compile()
{
	return buildVideoResources();
}

bool COpenGLPixelShader::buildVideoResources()
{
	return COpenGLShader::buildVideoResources(Driver->getShaderManageComponent()->getPSDir(), GL_FRAGMENT_SHADER_ARB);
}

COpenGLShader::COpenGLShader(const COpenGLDriver* driver, const char* name, const char* macroString)
	: Driver(driver), Name(name), MacroString(macroString), VideoBuilt(false)
{
	GLShader = 0;
}

COpenGLShader::~COpenGLShader()
{
	releaseVideoResources();
}

bool COpenGLShader::buildVideoResources(const char* dir, GLenum shaderType)
{
	if (VideoBuilt)
		return false;

	std::string absFileName = dir;
	normalizeDirName(absFileName);
	absFileName.append(Name);
	absFileName.append(".glsl");

	g_FileSystem->writeLog(ELOG_GX, "COpenGLShader buildVideoResources: %s, %s", absFileName.c_str(), MacroString.c_str());

	SShaderFile result;
	if (!CShaderUtil::loadFile_OpenGL(absFileName.c_str(), CShaderUtil::getShaderMacroSet(MacroString.c_str()), result))
	{
		g_FileSystem->writeLog(ELOG_GX, "GLSL shader failed to load: %s, %s", absFileName.c_str(), MacroString.c_str());
		return false;
	}

	const char* buffer = result.Buffer.data();

	GLhandleARB shaderHandle = Driver->GLExtension.extGlCreateShaderObject(shaderType);
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

	GLShader = shaderHandle;
	VideoBuilt = true;
	return true;
}

void COpenGLShader::releaseVideoResources()
{
	if (!VideoBuilt)
		return;

	Driver->GLExtension.extGlDeleteObject((GLhandleARB)GLShader);

	VideoBuilt = false;
}
