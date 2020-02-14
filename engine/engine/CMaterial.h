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

enum E_LIGHT_MODE : int
{
	ELM_ALWAYS = 0,		//no light
	ELM_FORWARD_BASE,
	ELM_FORWARD_ADD,
	ELM_SHADOW_CASTER,
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

class CMaterial;

class CPass
{
public:
	CPass(CMaterial* material, E_LIGHT_MODE mode) : Material(material), LightMode(mode)
	{
		Cull = ECM_BACK;
		AntiAliasing = EAAM_OFF;
		ZTest = ECFN_LESSEQUAL;
		ZWriteEnable = true;
		ColorMask = COLORWRITE_ALL;
		AlphaBlendEnabled = false;
		SrcBlend = EBF_ONE;
		DestBlend = EBF_ZERO;
	}

public:
	E_LIGHT_MODE	LightMode;

public:		//file macro
	std::string		VSFile;
	std::string		PSFile;
	std::set<std::string>		MacroSet;

public:		//render state
	E_CULL_MODE		Cull;
	E_ANTI_ALIASING_MODE		AntiAliasing;
	E_COMPARISON_FUNC		ZTest;
	bool		ZWriteEnable;
	E_COLOR_WRITE	ColorMask;
	E_BLEND_FACTOR	SrcBlend;
	E_BLEND_FACTOR	DestBlend;
	bool	AlphaBlendEnabled;

public:
	void clearMacroSet() { MacroSet.clear(); }
	void addMacro(const char* macro) { MacroSet.insert(macro); }
	std::string macroToString() const;
	void macroFromString(const char* macroString);

	const CMaterial* getMaterial() const { return Material; }

private:
	const CMaterial* Material;
};

struct STextureUnit
{
	ITexture* Texture;
	E_TEXTURE_CLAMP TextureWrapU;
	E_TEXTURE_CLAMP TextureWrapV;

	STextureUnit()
		: Texture(nullptr)
		, TextureWrapU(ETC_CLAMP)
		, TextureWrapV(ETC_CLAMP)
	{
	}
};

class CMaterial
{
public:
	CMaterial() : RenderQueue(ERQ_GEOMETRY) {}

public:
	void setVariable(const char* name, const float* src, uint32_t size);
	void setVariable(const char* name, const matrix4& mat) { setVariable(name, mat.M, 16); }
	void setVariable(const char* name, const vector4df& vec) { setVariable(name, &vec.x, 4); }
	void clearVariables() { ShaderVariableMap.clear(); }
	void setTexture(const char* name, ITexture* tex, E_TEXTURE_CLAMP wrapU = ETC_CLAMP, E_TEXTURE_CLAMP wrapV = ETC_CLAMP);
	void setMainTexture(ITexture* tex, E_TEXTURE_CLAMP wrapU = ETC_CLAMP, E_TEXTURE_CLAMP wrapV = ETC_CLAMP)
	{
		setTexture("_MainTex", tex, wrapU, wrapV);
	}

	const CPass* getPass(E_LIGHT_MODE mode) const;
	CPass* getPass(E_LIGHT_MODE mode);
	CPass& addPass(E_LIGHT_MODE lightMode);

public:
	int		RenderQueue;
	std::vector<CPass>	PassList;

public:
	std::map<std::string, std::vector<float>>	ShaderVariableMap;
	std::map<std::string, STextureUnit> TextureVariableMap;
};