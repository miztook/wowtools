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

enum E_RENDER_QUEUE : int
{
	ERQ_BACKGROUND = 1000,
	ERQ_GEOMETRY = 2000,
	ERQ_ALPHATEST = 2450,
	ERQ_GEOMETRYLAST = 2500,
	ERQ_TRANSPARENT = 3000,
	ERQ_OVERLAY = 4000,

	ERQ_INDEX_MIN = 0,
	ERQ_INDEX_MAX = 5000,
	ERQ_GEOMETRY_INDEX_MIN = ERQ_GEOMETRY - 500,
	ERQ_GEOMETRY_INDEX_MAX = ERQ_GEOMETRY + 500,
};

inline bool IsRenderQueueSolid(int queue)
{
	return queue < ERQ_ALPHATEST;
}

inline bool IsRenderQueueAlphaTest(int queue)
{
	return queue >= ERQ_ALPHATEST && queue < ERQ_TRANSPARENT;
}

inline bool isRenderQueueTransparent(int queue)
{
	return queue >= ERQ_TRANSPARENT && queue < ERQ_OVERLAY;
}

inline bool isRenderQueueOverlay(int queue)
{
	return queue >= ERQ_OVERLAY;
}

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
	E_RENDER_QUEUE	RenderQueue;

	std::string		VSFile;
	std::string		VSMacroString;
	std::string		PSFile;
	std::string		PSMacroString;
	std::map<std::string, std::vector<float>>	ShaderVariableMap;
	std::map<std::string, STextureUnit> TextureVariableMap;

	E_CULL_MODE		Cull;
	E_ANTI_ALIASING_MODE		AntiAliasing;
	E_COMPARISON_FUNC		ZBuffer;
	bool		ZWriteEnable;

	E_BLEND_TYPE	BlendType;	//blend desc
	E_COLOR_WRITE	colorWrite;

	SMaterial()
		: 
		RenderQueue(ERQ_GEOMETRY),
		BlendType(EMT_SOLID),
		colorWrite(COLORWRITE_ALL)
	{
		RenderQueue = ERQ_GEOMETRY;
		Cull = ECM_BACK;
		AntiAliasing = EAAM_OFF;
		ZBuffer = ECFN_LESSEQUAL;
		ZWriteEnable = true;
	}

	void setVariable(const char* name, const float* src, uint32_t size);
	void setVariable(const char* name, const matrix4& mat) { setVariable(name, mat.M, 16); }
	void setVariable(const char* name, const vector4df& vec) { setVariable(name, &vec.x, 4); }
	void clearVariables() { ShaderVariableMap.clear(); }
	void setTexture(const char* name, ITexture* tex, E_TEXTURE_CLAMP wrapU = ETC_CLAMP, E_TEXTURE_CLAMP wrapV = ETC_CLAMP);
	void setMainTexture(ITexture* tex, E_TEXTURE_CLAMP wrapU = ETC_CLAMP, E_TEXTURE_CLAMP wrapV = ETC_CLAMP)
	{
		setTexture("_MainTex", tex, wrapU, wrapV);
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

	switch (BlendType)
	{
	case EMT_SOLID:
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