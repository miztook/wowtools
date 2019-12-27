#pragma once

class IVertexBuffer;
class IIndexBuffer;
struct SMaterial;
class CCanvas;

class EngineUtil
{
public:
	static void drawDebugInfo(CCanvas* canvas, const char* strMsg);
};
