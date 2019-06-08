#include "game.h"
#include "Engine.h"

bool g_bExit = false;
bool g_bBackMode = false;

void MyMessageHandler::onSize(window_type hwnd, int width, int height)
{
	if (width == 0 || height == 0)					//minimized, stop
	{
		g_bBackMode = true;
	}
	else
	{
		g_bBackMode = false;

	}
}

CScene* createScene()
{
	CScene* scene = new CScene;


	return scene;
}

