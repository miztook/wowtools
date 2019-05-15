#include "mywowdriver.h"

#include "OpenGL\COpenGLDriver.h"
#include "OpenGL\COpenGLHelper.h"

IVideoDriver* COpenGL_Init(const SWindowInfo & wndInfo, E_DRIVER_TYPE driverType, bool vsync, E_AA_MODE aaMode);

IVideoDriver * mywow_InitDriver(const SWindowInfo & wndInfo, E_DRIVER_TYPE driverType, bool vsync, E_AA_MODE aaMode)
{
	switch (driverType)
	{
	case EDT_OPENGL:
		break;
	default:
		break;
	}

	return nullptr;
}


IVideoDriver* COpenGL_Init(const SWindowInfo & wndInfo, E_DRIVER_TYPE driverType, bool vsync, E_AA_MODE aaMode)
{
	if (!COpenGLHelper::init())
		return nullptr;

	COpenGLDriver* driver = new COpenGLDriver;
	if (!driver->initDriver(wndInfo, vsync, aaMode))
	{
		delete driver;
		driver = nullptr;
	}
	return driver;
}