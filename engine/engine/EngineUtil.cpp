#include "EngineUtil.h"
#include "Engine.h"
#include "CFontManager.h"
#include "CFTFont.h"
#include "IVertexIndexBuffer.h"
#include "SMaterial.h"
#include "ITexture.h"

void EngineUtil::drawDebugInfo(CCanvas* canvas, const char* strMsg)
{
	CFTFont* font = g_Engine->getFontManager()->getDefaultFont();
	ASSERT(font);
	font->drawA(canvas, strMsg, SColor::Green(), vector2di(5, 5));
}

