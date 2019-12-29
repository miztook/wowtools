#pragma once

#include "CMaterial.h"
#include "stringext.h"

struct SMaterial
{
	int	RenderQueue;

	std::string		VSFile;
	std::string		PSFile;
	std::set<std::string>		MacroSet;
	std::map<std::string, std::vector<float>>	ShaderVariableMap;
	std::map<std::string, STextureUnit> TextureVariableMap;

	E_CULL_MODE		Cull;
	E_ANTI_ALIASING_MODE		AntiAliasing;
	E_COMPARISON_FUNC		ZBuffer;
	bool		ZWriteEnable;

	E_COLOR_WRITE	ColorWrite;

	E_BLEND_FACTOR	SrcBlend;
	E_BLEND_FACTOR	DestBlend;
	bool	AlphaBlendEnabled;

	SMaterial()
		: 
		RenderQueue(ERQ_GEOMETRY),
		ColorWrite(COLORWRITE_ALL)
	{
		RenderQueue = ERQ_GEOMETRY;
		Cull = ECM_BACK;
		AntiAliasing = EAAM_OFF;
		ZBuffer = ECFN_LESSEQUAL;
		ZWriteEnable = true;
		AlphaBlendEnabled = false;
		SrcBlend = EBF_ONE;
		DestBlend = EBF_ZERO;
	}

	void clearMacroSet() { MacroSet.clear(); }
	void addMacro(const char* macro) { MacroSet.insert(macro); }
	std::string macroToString() const
	{
		std::string str;
		size_t count = 0;
		for (const auto& macro : MacroSet)
		{
			str += macro;
			if (count + 1 < MacroSet.size())
			{
				str += "#";
			}
			++count;
		}
		return str;
	}

	void macroFromString(const char* macroString)
	{
		std_string_split(macroString, '#', MacroSet);
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
};

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
