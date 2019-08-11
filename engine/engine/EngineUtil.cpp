#include "EngineUtil.h"
#include "Engine.h"
#include "CFontManager.h"
#include "CFTFont.h"

void EngineUtil::drawDebugInfo(const char* strMsg)
{
	CFTFont* font = g_Engine->getFontManager()->getDefaultFont();
	ASSERT(font);
	font->drawA(strMsg, SColor::White(), vector2di(5, 5));
}
