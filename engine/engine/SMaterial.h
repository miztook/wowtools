#pragma once

#include "SColor.h"
#include "base.h"
#include "vector4d.h"
#include "matrix4.h"
#include <string>
#include <set>
#include <vector>
#include <map>

class ITexture;

struct STextureUnit
{
	ITexture* Texture;
	E_TEXTURE_CLAMP TextureWrapU;
	E_TEXTURE_CLAMP TextureWrapV;

	STextureUnit()
		: Texture(nullptr), TextureWrapU(ETC_CLAMP), TextureWrapV(ETC_CLAMP)
	{
		
	}
};

struct SMRasterizerDesc
{
	E_CULL_MODE		Cull;
	E_ANTI_ALIASING_MODE		AntiAliasing;
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

	void Default()
	{
		srcBlend = EBF_ONE;
		destBlend = EBF_ZERO;
		alphaBlendEnabled = false;
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

	std::string		VSFile;
	std::string		VSMacroString;
	std::string		PSFile;
	std::string		PSMacroString;
	std::map<std::string, std::vector<float>>	ShaderVariableMap;
	std::map<std::string, STextureUnit> TextureVariableMap;

	SColorf		AmbientColor;
	SColorf		DiffuseColor;
	SColorf		EmissiveColor;

	SMRasterizerDesc	RasterizerDesc;
	SMDepthStencilDesc	DepthStencilDesc;

	bool		Lighting;
	bool		FogEnable;

	E_COLOR_WRITE	colorWrite;
	float		AlphaTestRef;

	SMaterial()
		: AmbientColor(1.0f, 1.0f, 1.0f, 1.0f),
		DiffuseColor(1.0f, 1.0f, 1.0f, 1.0f),
		EmissiveColor(1.0f, 1.0f, 1.0f, 1.0f),
		MaterialType(EMT_SOLID),
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

	bool isTransparent() const
	{
		return MaterialType > EMT_ALPHA_TEST &&
			MaterialType < EMT_COUNT;
	}

	bool isAlphaTest() const
	{
		return MaterialType == EMT_ALPHA_TEST;
	}

	void setVariable(const char* name, const float* src, uint32_t size);
	void setVariable(const char* name, const matrix4& mat) { setVariable(name, mat.M, 16); }
	void setVariable(const char* name, const vector4df& vec) { setVariable(name, &vec.x, 4); }
	void clearVariables() { ShaderVariableMap.clear(); }
	void setTexture(const char* name, ITexture* tex, E_TEXTURE_CLAMP wrapU, E_TEXTURE_CLAMP wrapV);

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
	case EMT_SOLID:
		break;
	case EMT_ALPHA_TEST:
		break;
	case EMT_TRANSPARENT_ALPHA_BLEND:
		desc.srcBlend = EBF_SRC_ALPHA;
		desc.destBlend = EBF_ONE_MINUS_SRC_ALPHA;
		desc.alphaBlendEnabled = true;
		break;
	case EMT_TRANSPARENT_ONE_ALPHA:
		desc.srcBlend = EBF_ONE;
		desc.destBlend = EBF_ONE_MINUS_SRC_ALPHA;
		desc.alphaBlendEnabled = true;
		break;
	case EMT_TRANSPARENT_ADD_ALPHA:
		desc.srcBlend = EBF_SRC_ALPHA;
		desc.destBlend = EBF_ONE;
		desc.alphaBlendEnabled = true;
		break;
	case EMT_TRANSPARENT_ADD_COLOR:
		desc.srcBlend = EBF_SRC_COLOR;
		desc.destBlend = EBF_ONE;
		desc.alphaBlendEnabled = true;
		break;
	case EMT_TRANSPARENT_MODULATE:
		desc.srcBlend = EBF_ZERO;
		desc.destBlend = EBF_SRC_COLOR;
		desc.alphaBlendEnabled = true;
		break;
	case EMT_TRANSPARENT_MODULATE_X2:
		desc.srcBlend = EBF_DST_COLOR;
		desc.destBlend = EBF_SRC_COLOR;
		desc.alphaBlendEnabled = true;
		break;
	case EMT_TRANSPARENT_ONE_ONE:
		desc.srcBlend = EBF_ONE;
		desc.destBlend = EBF_ONE;
		desc.alphaBlendEnabled = true;
		break;
	default:
		break;
	}
	return desc;
}

inline void SMaterial::setVariable(const char* name, const float* src, uint32_t size)
{
	auto itr = ShaderVariableMap.find(name);
	if (itr == ShaderVariableMap.end())
	{
		std::vector<float> buffer;
		buffer.resize(size);
		memcpy(buffer.data(), src, sizeof(float) * size);
		ShaderVariableMap[name] = buffer;
	}
	else
	{
		std::vector<float>& buffer = itr->second;
		ASSERT(buffer.size() >= size);
		memcpy(buffer.data(), src, sizeof(float) * size);
	}
}

inline void SMaterial::setTexture(const char* name, ITexture* tex, E_TEXTURE_CLAMP wrapU, E_TEXTURE_CLAMP wrapV)
{
	auto itr = TextureVariableMap.find(name);
	if (itr == TextureVariableMap.end())
	{
		STextureUnit texUnit;
		texUnit.Texture = tex;
		texUnit.TextureWrapU = wrapU;
		texUnit.TextureWrapV = wrapV;
		TextureVariableMap[name] = texUnit;
	}
	else
	{
		STextureUnit& texUnit = itr->second;
		texUnit.Texture = tex;
		texUnit.TextureWrapU = wrapU;
		texUnit.TextureWrapV = wrapV;
	}
}