#pragma once

#include "COpenGLPCH.h"

#include "base.h"
#include "SColor.h"
#include "rect.h"

class COpenGLExtension;
class ITexture;

class COpenGLMaterialRenderComponent
{
public:
	explicit COpenGLMaterialRenderComponent(const COpenGLExtension& extension);

public:
	void applyRenderStates();

	void setClearColor(SColor clearColor);
	void setViewport(recti vp);

	void setZWriteEnable(bool enable);
	bool getZWriteEnable() const;

	void setTextureFilter(uint32_t st, E_TEXTURE_FILTER filter, bool mipmap, bool isCube);
	void setTextureMipMap(uint32_t st, bool mipmap, bool isCube);
	void setSamplerTexture(uint32_t st, ITexture* tex, bool isCube);
	void setSamplerTextureMultiSample(uint32_t st, ITexture* tex);
	void setTextureWrap(uint32_t st, E_TEXTURE_ADDRESS address, E_TEXTURE_CLAMP wrap, bool isCube);

	void resetRSCache();

private:
	void setActiveTexture(uint32_t st);

private:
	struct SRenderStateCache
	{
		struct STextureUnit
		{
			ITexture*	texture;

			GLint	addressU;
			GLint	addressV;

			GLint	baseLevel;
			GLint	maxLevel;
			GLint	magFilter;
			GLint	minFilter;
			GLint	mipFilter;
			GLint	maxAniso;
		};

		SColorf		ClearColor;
		recti	Viewport;

		GLint		ActiveTextureIndex;

		GLint	 ZEnable;
		GLint	 ZFunc;
		GLint	 FillMode;
		GLboolean	 ColorMask[4];		//RGBA
		GLboolean	 ZWriteEnable;
		GLint	 StencilEnable;
		GLint	 ScissorEnable;
		GLint	 CullEnable;
		GLint	 CullMode;
		GLint	 FrontFace;
		GLint	 MultiSampleAntiAlias;
		GLint	 AntiAliasedLineEnable;

		GLint	 AlphaBlendEnable;
		GLint	 SrcBlend;
		GLint	 DestBlend;
		GLint	 AlphaFunc;
		GLfloat	 AlphaRef;
		GLfloat	 LineWidth;

		STextureUnit		TextureUnits[MATERIAL_MAX_TEXTURES];
	};

private:
	const COpenGLExtension&	Extension;

	SRenderStateCache	CurrentRenderState;

	SRenderStateCache	RsCache;
};