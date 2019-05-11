#pragma once

#include "SColor.h"
#include "base.h"
#include <string>
#include <set>

struct SMRasterizerDesc
{
	E_CULL_MODE		Cull;
	bool		Wireframe;
	bool		ScissorEnable;

	void Default()
	{
		Cull = ECM_NONE;
		Wireframe = false;
		ScissorEnable = false;
	}

	bool operator!=(const SMRasterizerDesc& b) const
	{
		bool equal = (Cull == b.Cull &&
			Wireframe == b.Wireframe &&
			ScissorEnable == b.ScissorEnable);
		return !equal;
	}
	bool operator==(const SMRasterizerDesc& b) const
	{
		bool equal = (Cull == b.Cull &&
			Wireframe == b.Wireframe &&
			ScissorEnable == b.ScissorEnable);
		return equal;
	}
};

struct SMDepthStencilDesc
{
	E_COMPARISON_FUNC		ZBuffer;
	bool		ZWriteEnable;
	bool		StencilEnable;

	void Default()
	{
		ZBuffer = ECFN_LESSEQUAL;
		ZWriteEnable = true;
		StencilEnable = false;
	}

	bool operator!=(const SMDepthStencilDesc& b) const
	{
		bool equal = (ZBuffer == b.ZBuffer &&
			ZWriteEnable == b.ZWriteEnable &&
			StencilEnable == b.StencilEnable);
		return !equal;
	}

	bool operator==(const SMDepthStencilDesc& b) const
	{
		bool equal = (ZBuffer == b.ZBuffer &&
			ZWriteEnable == b.ZWriteEnable &&
			StencilEnable == b.StencilEnable);
		return equal;
	}
};

struct SMRenderTargetBlendDesc
{
	E_BLEND_FACTOR	srcBlend;
	E_BLEND_FACTOR	destBlend;
	bool	alphaBlendEnabled;
	float	alphaTestRef;
	bool	alphaTestEnabled;

	void Default()
	{
		srcBlend = E_BLEND_FACTOR::One;
		destBlend = E_BLEND_FACTOR::Zero;
		alphaBlendEnabled = false;
		alphaTestRef = 0;
		alphaTestEnabled = false;
	}

	bool operator!=(const SMRenderTargetBlendDesc& b) const
	{
		bool equal = (srcBlend == b.srcBlend &&
			destBlend == b.destBlend &&
			alphaBlendEnabled == b.alphaBlendEnabled);
		return !equal;
	}

	bool operator==(const SMRenderTargetBlendDesc& b) const
	{
		bool equal = (srcBlend == b.srcBlend &&
			destBlend == b.destBlend &&
			alphaBlendEnabled == b.alphaBlendEnabled);
		return equal;
	}
};

struct SMaterial
{
	E_MATERIAL_TYPE	MaterialType;	//blend desc

	SColorf		AmbientColor;
	SColorf		DiffuseColor;
	SColorf		EmissiveColor;

	SMRasterizerDesc	RasterizerDesc;
	SMDepthStencilDesc	DepthStencilDesc;

	std::string		VSFile;
	std::set<std::string>  VSMacroSet;
	std::string		PSFile;
	std::set<std::string>  PSMacroSet;

	bool		Lighting;
	bool		FogEnable;

	E_COLOR_WRITE	colorWrite;
	float		AlphaTestRef;

	SMaterial()
		: AmbientColor(1.0f, 1.0f, 1.0f, 1.0f),
		DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f),
		EmissiveColor(1.0f, 1.0f, 1.0f, 1.0f),
		MaterialType(E_MATERIAL_TYPE::Solid),
		Lighting(true),
		FogEnable(false),
		colorWrite(COLORWRITE_ALL),
		AlphaTestRef(0)
	{
		RasterizerDesc.Default();
		DepthStencilDesc.Default();
	}

	float getMaterialAlpha() const
	{
		if (Lighting)
			return DiffuseColor.a;
		else
			return EmissiveColor.a;
	}

	void setMaterialAlpha(float alpha)
	{
		if (Lighting)
			DiffuseColor.a = (DiffuseColor.a * alpha);
		else
			EmissiveColor.a = (EmissiveColor.a * alpha);
	}

	SMRenderTargetBlendDesc getRenderTargetBlendDesc() const;
};

struct SGlobalMaterial
{
	SGlobalMaterial()
	{
		TextureFilter = ETF_TRILINEAR;
		MipMapLodBias = 0;
	}

	E_TEXTURE_FILTER TextureFilter;
	int MipMapLodBias;
};

inline SMRenderTargetBlendDesc SMaterial::getRenderTargetBlendDesc() const
{
	SMRenderTargetBlendDesc desc;
	desc.Default();

	switch (MaterialType)
	{
	case E_MATERIAL_TYPE::Solid:
		break;
	case E_MATERIAL_TYPE::AlphaTest:
		desc.alphaTestEnabled = true;
		desc.alphaTestRef = AlphaTestRef * getMaterialAlpha();
		break;
	case E_MATERIAL_TYPE::Transparent_AlphaBlend:
		desc.srcBlend = E_BLEND_FACTOR::Src_Alpha;
		desc.destBlend = E_BLEND_FACTOR::One_Minus_Src_Alpha;
		desc.alphaBlendEnabled = true;
		break;
	case E_MATERIAL_TYPE::Transparent_One_Alpha:
		desc.srcBlend = E_BLEND_FACTOR::One;
		desc.destBlend = E_BLEND_FACTOR::One_Minus_Src_Alpha;
		desc.alphaBlendEnabled = true;
		break;
	case E_MATERIAL_TYPE::Transparent_Add_Alpha:
		desc.srcBlend = E_BLEND_FACTOR::Src_Alpha;
		desc.destBlend = E_BLEND_FACTOR::One;
		desc.alphaBlendEnabled = true;
		break;
	case E_MATERIAL_TYPE::Transparent_Add_Color:
		desc.srcBlend = E_BLEND_FACTOR::Src_Color;
		desc.destBlend = E_BLEND_FACTOR::One;
		desc.alphaBlendEnabled = true;
		break;
	case E_MATERIAL_TYPE::Transparent_Modulate:
		desc.srcBlend = E_BLEND_FACTOR::Zero;
		desc.destBlend = E_BLEND_FACTOR::Src_Color;
		desc.alphaBlendEnabled = true;
		break;
	case E_MATERIAL_TYPE::Transparent_Modulate_X2:
		desc.srcBlend = E_BLEND_FACTOR::Dst_Color;
		desc.destBlend = E_BLEND_FACTOR::Src_Color;
		desc.alphaBlendEnabled = true;
		break;
	case E_MATERIAL_TYPE::Transparent_One_One:
		desc.srcBlend = E_BLEND_FACTOR::One;
		desc.destBlend = E_BLEND_FACTOR::One;
		desc.alphaBlendEnabled = true;
		break;
	default:
		break;
	}
	return desc;
}