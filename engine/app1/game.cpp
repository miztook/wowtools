#include "game.h"
#include "Engine.h"
#include "function3d.h"

CGame* g_pGame = nullptr;

void MyMessageHandler::onExit(window_type hwnd)
{
	g_pGame->m_bExit = true;
}

void MyMessageHandler::onSize(window_type hwnd, int width, int height)
{
	if (width == 0 || height == 0)					//minimized, stop
	{
		g_pGame->m_bBackMode = true;
	}
	else
	{
		g_pGame->m_bBackMode = false;

		dimension2d screenSize((uint32_t)width, (uint32_t)height);
		g_Engine->onWindowResized(screenSize);

		if (g_pGame->m_pScene)
			g_pGame->m_pScene->onScreenResize(screenSize);
	}
}

CGame::CGame()
{
	m_bExit = false;
	m_bBackMode = false;
	m_pScene = nullptr;
}

CGame::~CGame()
{
	delete m_pScene;
	m_pScene = nullptr;
}

void CGame::createScene()
{
	CScene* scene = new CScene("default");

	const auto& dim = g_Engine->getDriver()->getDisplayMode();
	float aspect = (float)dim.width / dim.height;
	vector3df camPos(0, 5, 10);
	vector3df camDir = f3d::normalize(vector3df(0, 2.5f, 0) - camPos);
	scene->init3DCamera(PI / 4, aspect, 1, 2000.0f, camPos, camDir, vector3df::UnitZ());

	scene->init2DCamera(dim, 0, 1);

	m_pScene = scene;
}

void CGame::update()
{

}

void createGame()
{
	g_pGame = new CGame;
}

void destroyGame()
{
	delete g_pGame;
	g_pGame = nullptr;
}
