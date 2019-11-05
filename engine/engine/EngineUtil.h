#pragma once

class IVertexBuffer;
class IIndexBuffer;
struct SMaterial;

class EngineUtil
{
public:
	static void drawDebugInfo(const char* strMsg);

	static void buildVideoResources(IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, const SMaterial* material);
};
