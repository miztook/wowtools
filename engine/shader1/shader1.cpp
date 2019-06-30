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

	shaderManagerComponent->getVertexShader("Default_P");
	shaderManagerComponent->getVertexShader("Default_PC");
	shaderManagerComponent->getVertexShader("Default_PCT");
	shaderManagerComponent->getVertexShader("Default_PN");
	shaderManagerComponent->getVertexShader("Default_PNC");
	shaderManagerComponent->getVertexShader("Default_PNCT");
	shaderManagerComponent->getVertexShader("Default_PNT");
	shaderManagerComponent->getVertexShader("Default_PT");
	
	shaderManagerComponent->getPixelShader("Default_P");
	//const CGLProgram* program = shaderManagerComponent->getGlProgram(vs, ps);
}

