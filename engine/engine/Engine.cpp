#include "Engine.h"
#include "mywow.h"
#include "CFileSystem.h"
#include "wowEnvironment.h"
#include "wowDatabase.h"

CFileSystem* g_FileSystem = nullptr;
wowEnvironment* g_WowEnvironment = nullptr;
wowDatabase* g_WowDatabase = nullptr;
Engine* g_Engine = nullptr;


bool createFileSystem(const char* baseDir, const char* wowDir)
{
	g_FileSystem = new CFileSystem(baseDir, wowDir);

	return true;
}

void destroyFileSystem()
{
	delete g_FileSystem;
	g_FileSystem = nullptr;
}

bool createWowEnvironment(CFileSystem * fs, bool loadCascFile)
{
	g_WowEnvironment = new wowEnvironment(fs);
	if (!g_WowEnvironment->init())
	{
		delete g_WowEnvironment;
		g_WowEnvironment = nullptr;
		return false;
	}
	
	if (loadCascFile)
	{
		if (!g_WowEnvironment->loadCascListFiles())
			return false;
	}
	return true;
}

void destroyWowEnvironment()
{
	delete g_WowEnvironment;
	g_WowEnvironment = nullptr;
}

bool createWowDatabase(const wowEnvironment * wowEnv)
{
	g_WowDatabase = new wowDatabase(wowEnv);
	if (!g_WowDatabase->init())
		return false;

	return true;
}

void destroyWowDatabase()
{
}

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
