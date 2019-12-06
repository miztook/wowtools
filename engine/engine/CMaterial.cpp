#include "CMaterial.h"

SMRenderTargetBlendDesc getRenderTargetBlendDesc(E_BLEND_TYPE blendType)
{
	SMRenderTargetBlendDesc desc;

	switch (blendType)
	{
	case EMT_SOLID:
		desc.srcBlend = EBF_ONE;
		desc.destBlend = EBF_ZERO;
		desc.alphaBlendEnabled = false;
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
