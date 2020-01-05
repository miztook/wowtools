#pragma once

#include "base.h"

class CCanvas;
class IRenderer;

class EngineUtil
{
public:
	static void drawDebugInfo(CCanvas* canvas, const char* strMsg);

	static bool hasLight(const IRenderer* renderer);
};
