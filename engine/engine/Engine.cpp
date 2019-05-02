#include "Engine.h"
#include "mywow.h"

Engine* g_Engine = nullptr;

Engine::Engine(const SWindowInfo& wndInfo)
{
	WindowInfo = wndInfo;
}

Engine::~Engine()
{
}

bool createEngine(const SWindowInfo& wndInfo, const SRendererInfo& rendererInfo)
{
	g_Engine = new Engine(wndInfo);

	return true;
}

void destroyEngine()
{
	delete g_Engine;
	g_Engine = nullptr;
}
