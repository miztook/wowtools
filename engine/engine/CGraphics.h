#pragma once

#include "base.h"
#include "RenderStruct.h"
#include "vector2d.h"
#include "rect.h"
#include "SColor.h"
#include "IVideoDriver.h"

//¸¨Öúº¯Êý

class ITexture;

class CGraphics
{
public:
	explicit CGraphics(IVideoDriver* driver);

public:
	void draw2DImage(ITexture* texture, const vector2di& pos,
		const recti* sourceRect = nullptr,
		SColor color = SColor::White(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());

private:
	IVideoDriver* Driver;
};
