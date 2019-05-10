#pragma once

#include "SColor.h"
#include <string>
#include <set>

enum class E_MATERIAL_TYPE : int
{
	Solid = 0,

	AlphaTest,

	Transparent_AlphaBlend_Test,
	Transparent_AlphaBlend,
	Transparent_One_Alpha,
	Transparent_Add_Alpha,
	Transparent_Add_Color,

	Transparent_Modulate,
	Transparent_Modulate_X2,
	Transparent_One_One,

	MAX,
};

enum class E_CULL_MODE : int
{
	None,
	Front,
	Back,
};

enum class E_COMPARISON_FUNC : int
{
	Never = 0,
	LessEqual,
	Equal ,
	Less,
	NotEqual,
	GreatEqual,
	Greater,
	Always,
};

enum E_COLOR_WRITE : uint8_t
{
	COLORWRITE_RED = 1,
	COLORWRITE_GREEN = 2,
	COLORWRITE_BLUE = 4,
	COLORWRITE_ALPHA = 8,
	COLORWRITE_RGBA = 0xf,
};

enum class E_BLEND_OP : int
{
	Add = 0,
	Substract,
};

enum class E_BLEND_FACTOR : int
{
	Zero = 0,
	One,
	Dst_Color,
	One_Minus_Dst_Color,
	Src_Color,
	One_Minus_Src_Color,
	Src_Alpha,
	One_Minus_Src_Alpha,
	Dst_Alpha,
	One_Minus_Dst_Alpha,
	Src_Alpha_Saturate,
};

enum class E_TEXTURE_FILTER : int
{
	None = 0,
	Bilinear,
	Trilinear,
	Anisotropic_x1,
	Anisotropic_x2,
	Anisotropic_x4,
	Anisotropic_x8,
	Anisotropic_x16,
};

struct SMRasterizerDesc
{
	E_CULL_MODE		Cull;
	bool		Wireframe;
	bool		ScissorEnable;

	void Default()
	{
		Cull = E_CULL_MODE::None;
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
		ZBuffer = E_COMPARISON_FUNC::LessEqual;
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
	E_COLOR_WRITE	colorWrite;
	E_BLEND_OP		blendOp;
	E_BLEND_FACTOR	srcBlend;
	E_BLEND_FACTOR	destBlend;
	bool	alphaBlendEnabled;
	float	alphaTestRef;
	bool	alphaTestEnabled;

	void Default()
	{
		colorWrite = COLORWRITE_RGBA;
		blendOp = E_BLEND_OP::Add;
		srcBlend = E_BLEND_FACTOR::One;
		destBlend = E_BLEND_FACTOR::Zero;
		alphaBlendEnabled = false;
		alphaTestRef = 1.0f;
		alphaTestEnabled = false;
	}

	bool operator!=(const SMRenderTargetBlendDesc& b) const
	{
		bool equal = (colorWrite == b.colorWrite &&
			blendOp == b.blendOp &&
			srcBlend == b.srcBlend &&
			destBlend == b.destBlend &&
			alphaBlendEnabled == b.alphaBlendEnabled);
		return !equal;
	}

	bool operator==(const SMRenderTargetBlendDesc& b) const
	{
		bool equal = (colorWrite == b.colorWrite &&
			blendOp == b.blendOp &&
			srcBlend == b.srcBlend &&
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

	float		AlphaTestRef;

	SMRenderTargetBlendDesc getRenderTargetBlendDesc() const;
};

struct SGlobalMaterial
{
	SGlobalMaterial()
	{
		TextureFilter = E_TEXTURE_FILTER::Trilinear;
		MipMapLodBias = 0;
	}

	E_TEXTURE_FILTER TextureFilter;
	int MipMapLodBias;
};

SMRenderTargetBlendDesc SMaterial::getRenderTargetBlendDesc() const
{

}