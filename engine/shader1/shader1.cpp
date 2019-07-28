#include "base.h"
#include "mywow.h"
#include "mywowdriver.h"

#include "OpenGL/COpenGLDriver.h"
#include "OpenGL/COpenGLShaderManageComponent.h"

#pragma comment(lib, "mywow.lib")
#pragma comment(lib, "mywowdriver.lib")
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

	createFileSystem(CFileSystem::getWorkingDirectory().c_str(), R"(E:\World Of Warcraft)");
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
 		"Default_P",
		"Default_PC",
		"Default_PCT",
		"Default_PN",
		"Default_PNC",
		"Default_PNCT",
		"Default_PNT",
		"Default_PT",
	};

	for (const char* vsFile : vsshaders)
	{
		shaderManagerComponent->getVertexShader(vsFile);
	}
	
	const char* psshaders[] =
	{
		"Default_P",
		"Default_PC",
		"Default_PCT",
		"Default_PN",
		"Default_PNC",
		"Default_PNCT",
		"Default_PNT",
		"Default_PT",
		"UI",
		"UI_Alpha",
		"UI_Alpha_AlphaChannel",
		"UI_AlphaChannel",
	};
	
	for (const char* psFile : psshaders)
	{
		shaderManagerComponent->getPixelShader(psFile);
	}
	//const CGLProgram* program = shaderManagerComponent->getGlProgram(vs, ps);
}

