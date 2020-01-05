#include "EngineUtil.h"
#include "Engine.h"
#include "CFontManager.h"
#include "CFTFont.h"
#include "CLightSetting.h"
#include "IRenderer.h"
#include "ISceneNode.h"

void EngineUtil::drawDebugInfo(CCanvas* canvas, const char* strMsg)
{
	CFTFont* font = g_Engine->getFontManager()->getDefaultFont();
	ASSERT(font);
	font->drawA(canvas, strMsg, SColor::Green(), vector2di(5, 5));
}

bool EngineUtil::hasLight(const IRenderer* renderer)
{
	return g_Engine->getLightSetting()->hasLight(renderer->getSceneNode()->getLayer());
}

