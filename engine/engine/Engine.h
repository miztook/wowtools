#pragma once
#include "base.h"
#include "COSInfo.h"
#include "CTimer.h"
#include "CInputReader.h"
#include "IVideoDriver.h"

class CFontManager;
class CMeshManager;

using driverInitFunc = std::function<IVideoDriver*(const SWindowInfo& wndInfo,
	E_DRIVER_TYPE driverType, bool vsync, E_AA_MODE aaMode)>;

//
bool createEngine(const SWindowInfo& wndInfo, E_DRIVER_TYPE driverType, bool vsync, E_AA_MODE aaMode, driverInitFunc initFunc);
void destroyEngine();

class Engine
{
public:
	explicit Engine(const SWindowInfo& wndInfo);
	~Engine();

public:
	bool init(E_DRIVER_TYPE driverType, bool vsync, E_AA_MODE aaMode, driverInitFunc initFunc);

	void setMessageHandler(IMessageHandler* messageHandler) { MessageHandler = messageHandler; }
	IMessageHandler* getMessageHandler() { return MessageHandler; }

	void onWindowResized(const dimension2d& size);

public:
	IVideoDriver* getDriver() const { return Driver; }
	const SWindowInfo& getWindowInfo() const { return WindowInfo; }
	const COSInfo&	getOSInfo() const { return OSInfo; }
	CInputReader* getInputReader() { return &InputReader; }
	CFontManager* getFontManager() { return FontManager; }

private:
	IVideoDriver*	Driver;
	SWindowInfo		WindowInfo;
	COSInfo		OSInfo;
	CTimer		Timer;
	CInputReader		InputReader;
	IMessageHandler*		MessageHandler;

	//
	CFontManager*		FontManager;
	CMeshManager*		MeshManager;
};

extern Engine* g_Engine;
