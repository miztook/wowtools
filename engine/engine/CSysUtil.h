#pragma once

#include "base.h"
#include "vector2d.h"

class CSysUtility
{
public:
	static bool isKeyPressed(uint8_t vk);

	static bool getCursorPosition(vector2di& pos);

	static bool openURLtoJsonFile(const char* url, const char* filename);

	static SWindowInfo createWindow(const char* caption, uint32_t width, uint32_t height, bool fullscreen, bool hide);

	static void messageBoxWarning(const char* msg);

	static void outputDebug(const char* format, ...);

	static void exitProcess(int ret);

};