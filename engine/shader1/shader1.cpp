#include "base.h"
#include "mywow.h"
#include "mywowdriver.h"
#include "CShaderUtil.h"

#include "OpenGL/COpenGLDriver.h"
#include "OpenGL/COpenGLShaderManageComponent.h"

#pragma comment(lib, "mywow.lib")
#pragma comment(lib, "mywowdriver.lib")
#pragma comment(lib, "CascLib.lib")
#pragma comment(lib, "freetype.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

void compileShaders();

int main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//_CrtSetBreakAlloc(336);

	SWindowInfo wndInfo = CSysUtil::createWindow("shader1", 1136, 640, false, false);
	HWND hwnd = wndInfo.hwnd;

	createFileSystem(R"(E:\World Of Warcraft)");
	if (!createEngine(wndInfo, EDT_OPENGL, true, E_AA_FXAA, mywow_InitDriver))
	{
		destroyEngine();
		return -1;
	}

	compileShaders();

	destroyEngine();
	destroyFileSystem();

	return 0;
}

void compileShaders()
{
	//
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderManageComponent* shaderManagerComponent = driver->getShaderManageComponent();

	const char* vsshaders[] =
	{
		"Default_PC",
		"Default_PCT",
		"Default_PNC",
		"Default_PNCT",
	};

	for (const char* vsFile : vsshaders)
	{
		shaderManagerComponent->getVertexShader(vsFile);
	}
	
	const char* psshaders[] =
	{
		"Default_PC",
		"Default_PCT",
		"Default_PNC",
		"Default_PNCT",
	};
	
	for (const char* psFile : psshaders)
	{
		shaderManagerComponent->getPixelShader(psFile);
	}

	shaderManagerComponent->getPixelShader("UI", CShaderUtil::getShaderMacroString("").c_str());
	shaderManagerComponent->getPixelShader("UI", CShaderUtil::getShaderMacroString("_USE_ALPHA_").c_str());
	shaderManagerComponent->getPixelShader("UI", CShaderUtil::getShaderMacroString("_USE_ALPHA_CHANNEL_").c_str());
	shaderManagerComponent->getPixelShader("UI", CShaderUtil::getShaderMacroString("_USE_ALPHA_", "_USE_ALPHA_CHANNEL_").c_str());

	//const CGLProgram* program = shaderManagerComponent->getGlProgram(vs, ps);
}

