#include "base.h"
#include "mywow.h"
#include "mywowdriver.h"

#include "OpenGL/COpenGLDriver.h"
#include "OpenGL/COpenGLShaderManageComponent.h"

#pragma comment(lib, "mywow.lib")
#pragma comment(lib, "mywowdriver.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

int main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//_CrtSetBreakAlloc(212);

	SWindowInfo wndInfo = CSysUtil::createWindow("app1", 1136, 640, false, false);
	HWND hwnd = wndInfo.hwnd;

	createFileSystem(CFileSystem::getWorkingDirectory().c_str(), R"(E:\World Of Warcraft)");

	if (!createEngine(wndInfo, EDT_OPENGL, true, E_AA_FXAA, mywow_InitDriver))
	{
		destroyEngine();
		return -1;
	}

	//
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderManageComponent* shaderManagerComponent = driver->getShaderManageComponent();

	destroyEngine();

	destroyFileSystem();

	return 0;
}