#pragma once

#include "COpenGLPCH.h"

#include "base.h"
#include "SColor.h"
#include "rect.h"
#include "CMaterial.h"

class COpenGLDriver;
class ITexture;
class CGLProgram;

class COpenGLMaterialRenderComponent
{
public:
	explicit COpenGLMaterialRenderComponent(const COpenGLDriver* driver);

public:
	bool init();

	void setRenderStates(const CPass* pass, const SGlobalMaterial* globalMaterial, const CGLProgram* program);
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

			GLint	addressS;
			GLint	addressT;

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

		GLint	 ZTest;
		GLint	 ZFunc;
		GLint	 FillMode;
		GLboolean	 ColorMask[4];		//RGBA
		GLboolean	 ZWrite;
		//GLint	 StencilEnable;
		//GLint	 ScissorEnable;
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

		STextureUnit		TextureUnits[MATERIAL_MAX_TEXTURES];
	};

private:
	const COpenGLDriver* Driver;

	SRenderStateCache	CurrentRenderState;

	SRenderStateCache	RsCache;
};