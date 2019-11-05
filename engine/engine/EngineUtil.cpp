#include "EngineUtil.h"
#include "Engine.h"
#include "CFontManager.h"
#include "CFTFont.h"
#include "IVertexIndexBuffer.h"
#include "SMaterial.h"
#include "ITexture.h"

void EngineUtil::drawDebugInfo(const char* strMsg)
{
	CFTFont* font = g_Engine->getFontManager()->getDefaultFont();
	ASSERT(font);
	font->drawA(strMsg, SColor::Green(), vector2di(5, 5));
}

void EngineUtil::buildVideoResources(IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, const SMaterial* material)
{
	if (vbuffer)
		IVideoResource::buildVideoResources(vbuffer);
	if (ibuffer)
		IVideoResource::buildVideoResources(ibuffer);

	for (const auto& kv : material->TextureVariableMap)
	{
		ITexture* tex = kv.second.Texture;
		if (tex)
			IVideoResource::buildVideoResources(tex);
	}
}
