#include "base.h"
#include "mywow.h"
#include "mywowdriver.h"

#include "game.h"
#include "CWinMiniDump.h"
#include "CWinMemDbg.h"

#pragma comment(lib, "mywow.lib")
#pragma comment(lib, "mywowdriver.lib")
#pragma comment(lib, "freetype.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(linker, "/subsystem:windows /ENTRY:mainCRTStartup")

CWinMemDbg globalDbg;

int doRun();

int main()
{
	CWinMiniDump::begin();

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	_CrtSetBreakAlloc(174);

	globalDbg.beginCheckPoint();

	int ret = doRun();

	bool safe = globalDbg.endCheckPoint();
	ASSERT(safe);

	globalDbg.outputMaxMemoryUsed();

	CWinMiniDump::end();

	return ret;
}

int doRun()
{
	SWindowInfo wndInfo = CSysUtil::createWindow("app1", 1136, 640, false, false);
	HWND hwnd = wndInfo.hwnd;

	createFileSystem(R"(E:\World Of Warcraft)");

	if (!createEngine(wndInfo, EDT_OPENGL, true, E_AA_FXAA, mywow_InitDriver))
	{
		destroyEngine();
		return -1;
	}

	createGame();
	g_pGame->createInput();

	MyMessageHandler handler;
	g_Engine->setMessageHandler(&handler);

	g_pGame->createScene();
	CSceneRenderer* sceneRenderer = CSceneRenderer::createSceneRenderer();

	MSG msg;
	while (!g_pGame->m_bExit)
	{
		if (::PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
		{
			bool active = !g_pGame->m_bBackMode && ::GetActiveWindow() == hwnd;
			if (active)
				g_pGame->update();

			//render scene
			sceneRenderer->renderFrame(g_pGame->m_pScene, active);

			g_pGame->m_pScene->afterFrame();		//cleanup

			::Sleep(1);
		}
	}

	delete sceneRenderer;

	g_pGame->destroyInput();
	destroyGame();

	destroyEngine();

	destroyFileSystem();

	return 0;
}