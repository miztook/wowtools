#pragma once
#include "base.h"
#include "COSInfo.h"
#include "CTimer.h"
#include "CInputReader.h"
#include "IVideoDriver.h"

class CFileSystem;
class wowEnvironment;
class wowDatabase;

//
bool createFileSystem(const char* baseDir, const char* wowDir);
void destroyFileSystem();

bool createWowEnvironment(CFileSystem* fs, bool loadCascFile);
void destroyWowEnvironment();

bool createWowDatabase(const wowEnvironment* wowEnv);
void destroyWowDatabase();

bool createEngine(const SWindowInfo& wndInfo, E_DRIVER_TYPE driverType, bool vsync, E_AA_MODE aaMode);
void destroyEngine();


class Engine
{
public:
	explicit Engine(const SWindowInfo& wndInfo);
	~Engine();

public:
	bool initDriver(E_DRIVER_TYPE driverType, bool vsync, E_AA_MODE aaMode);

	void setMessageHandler(IMessageHandler* messageHandler) { MessageHandler = messageHandler; }
	IMessageHandler* getMessageHandler() { return MessageHandler; }

public:
	const SWindowInfo& getWindowInfo() const { return WindowInfo; }
	const COSInfo&	getOSInfo() const { return OSInfo; }
	CInputReader* getInputReader() { return &InputReader; }

private:
	IVideoDriver*	Driver;
	SWindowInfo		WindowInfo;
	COSInfo		OSInfo;
	CTimer		Timer;
	CInputReader		InputReader;
	IMessageHandler*		MessageHandler;
};

extern Engine* g_Engine;
extern CFileSystem* g_FileSystem;
extern wowEnvironment* g_WowEnvironment;
extern wowDatabase* g_WowDatabase;