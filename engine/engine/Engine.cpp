#include "Engine.h"
#include "mywow.h"

Engine* g_Engine = nullptr;

bool createEngine(const SWindowInfo& wndInfo, E_DRIVER_TYPE driverType, bool vsync, E_AA_MODE aaMode, driverInitFunc initFunc)
{
	g_Engine = new Engine(wndInfo);

	if (!g_Engine->init(driverType, vsync, aaMode, initFunc))
	{
		goto fail;
	}

	return true;

fail:
	ASSERT(false);
	return false;
}

void destroyEngine()
{
	delete g_Engine;
	g_Engine = nullptr;
}


Engine::Engine(const SWindowInfo& wndInfo)
{
	WindowInfo = wndInfo;

	Driver = nullptr;
}

Engine::~Engine()
{
	delete Driver;
}

bool Engine::init(E_DRIVER_TYPE driverType, bool vsync, E_AA_MODE aaMode, driverInitFunc initFunc)
{
	if (WindowInfo.width == 0 || WindowInfo.height == 0 || !initFunc)
	{
		goto fail;
	}

	Driver = initFunc(WindowInfo, driverType, vsync, aaMode);
	if (!Driver)
	{
		goto fail;
	}

	return true;

fail:
	ASSERT(false);
	return false;
}
