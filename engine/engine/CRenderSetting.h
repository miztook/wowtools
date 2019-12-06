#pragma once

#include "SMaterial.h"

class CRenderSetting
{
public:
	CRenderSetting()
	{
		//
		GlobalMaterial3D.TextureFilter = ETF_TRILINEAR;
		GlobalMaterial3D.MipMapLodBias = 0;

		GlobalMaterial2D.MipMapLodBias = 0;
		GlobalMaterial2D.TextureFilter = ETF_NONE;
	}

public:
	const SGlobalMaterial& getGlobalMaterial3D() const { return GlobalMaterial3D; }
	const SGlobalMaterial& getGlobalMaterial2D() const { return GlobalMaterial2D; }

	SGlobalMaterial& getGlobalMaterial3D() { return GlobalMaterial3D; }
	SGlobalMaterial& getGlobalMaterial2D() { return GlobalMaterial2D; }

private:
	SGlobalMaterial	GlobalMaterial3D;
	SGlobalMaterial	GlobalMaterial2D;
};


