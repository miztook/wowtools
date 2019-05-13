#include "Engine.h"
#include "mywow.h"

Engine* g_Engine = nullptr;

bool createEngine(const SWindowInfo& wndInfo, E_DRIVER_TYPE driverType, bool vsync, E_AA_MODE aaMode)
{
	g_Engine = new Engine(wndInfo);

	return true;
}

void destroyEngine()
{
	delete g_Engine;
	g_Engine = nullptr;
}


Engine::Engine(const SWindowInfo& wndInfo)
{
	WindowInfo = wndInfo;
}

Engine::~Engine()
{
}
