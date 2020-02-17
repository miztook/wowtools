#include "CMaterial.h"
#include "CShaderUtil.h"

void CMaterial::setVariable(const char* name, const float* src, uint32_t size)
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

void CMaterial::setTexture(const char* name, ITexture* tex, E_TEXTURE_CLAMP wrapU /*= ETC_CLAMP*/, E_TEXTURE_CLAMP wrapV /*= ETC_CLAMP*/)
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

const CPass* CMaterial::getPass(E_LIGHT_MODE mode) const
{
	for (const auto& pass : PassList)
	{
		if (pass.LightMode == mode)
			return &pass;
	}
	return nullptr;
}

CPass* CMaterial::getPass(E_LIGHT_MODE mode)
{
	for (auto& pass : PassList)
	{
		if (pass.LightMode == mode)
			return &pass;
	}
	return nullptr;
}

CPass* CMaterial::addPass(E_LIGHT_MODE lightMode)
{
	CPass pass(this, lightMode);
	pass.LightMode = lightMode;
	PassList.push_back(pass);
	return &PassList.back();
}

