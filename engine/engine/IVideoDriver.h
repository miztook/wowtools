#pragma once

#include "base.h"
#include "rect.h"
#include "vector2d.h"
#include "matrix4.h"
#include "SMaterial.h"
#include "IRenderTarget.h"
#include "SColor.h"
#include "RenderStruct.h"
#include "S3DVertex.h"
#include <string>
#include <list>

class ITexture;
class ITextureWriter;

struct SDriverSetting
{
	SDriverSetting()
	{
		vsync = true;
		aaMode = E_AA_MSAA_1;
		antialias = 1;
	}

	E_AA_MODE aaMode;
	bool vsync;
	uint8_t antialias;
};

struct SDisplayMode
{
	SDisplayMode()
	{
		width = 0;
		height = 0;
		refreshrate = 60;
	}

	uint16_t width;
	uint16_t height;
	uint32_t refreshrate;

	bool operator==(const SDisplayMode& mode) const
	{
		return width == mode.width &&
			height == mode.height &&
			refreshrate == mode.refreshrate;
	}
};

class CAdapterInfo
{
public:
	CAdapterInfo() : vendorID(0)
	{
	}

public:
	uint32_t		vendorID;
	std::string		description;
	std::string		name;
	std::string		vendorName;

	typedef std::list<SDisplayMode>	T_DisplayModeList;
	T_DisplayModeList displayModes;

	void addDisplayMode(const SDisplayMode& mode)
	{
		if (displayModes.end() == std::find(displayModes.begin(), displayModes.end(), mode))
		{
			displayModes.push_back(mode);
		}
	}

	SDisplayMode getCloseMatchDisplayMode(uint32_t width, uint32_t height) const
	{
		SDisplayMode ret;
		//select first
		T_DisplayModeList::const_iterator itr = displayModes.begin();
		uint32_t diff = abs(((int)itr->width - (int)width)) + abs(((int)itr->height - (int)height));
		ret = (*itr);

		++itr;
		for (; itr != displayModes.end(); ++itr)
		{
			uint32_t d = abs(((int)itr->width - (int)width)) + abs(((int)itr->height - (int)height));
			if (d < diff)
			{
				diff = d;
				ret = (*itr);
			}
		}

		return ret;
	}
};

class IVideoDriver
{
private:
	DISALLOW_COPY_AND_ASSIGN(IVideoDriver);

public:
	explicit IVideoDriver(E_DRIVER_TYPE type)
		: DriverType(type)
	{
		CurrentRenderTarget = nullptr;

		IsSupportDepthTexture = false;
		IsMultiSampleEnabled = false;
		IsSupportA8L8 = false;

		PrimitivesDrawn = 0;
		DrawCall = 0;

		OrthoCenterOffset.set(0, 0);
	}

	virtual ~IVideoDriver() {}

public:
	E_DRIVER_TYPE getDriverType() const { return DriverType; }

	const vector2df& getOrthoCenterOffset() const { return OrthoCenterOffset; }
	const recti& getViewPort() const { return Viewport; }
	const dimension2d& getDisplayMode() const { return ScreenSize; }
	const SDriverSetting& getDriverSetting() const { return DriverSetting; }

	void setMaterial(const SMaterial& material) { Material = &material; }
	const SMaterial* getMaterial() const { return Material; }
	void setGlobalMaterial(const SGlobalMaterial& globalMaterial) { GlobalMaterial = &globalMaterial; }
	const SGlobalMaterial* getGlobalMaterial() const { return GlobalMaterial; }

	const matrix4& getView2DTM() const { return View2DTM; }
	const matrix4& getProject2DTM() const { return Project2DTM; }
	const matrix4& getVPScaleTM() const { return VPScaleMatrix; }
	const matrix4& getInvVPScaleTM() const { return InvVPScaleMatrix; }

	IRenderTarget* getFrameBufferRT() const { return FrameBufferRT.get(); }

	bool isFXAAEnabled() const { return DriverSetting.aaMode >= E_AA_FXAA && DriverSetting.aaMode <= E_AA_FXAA; }
	bool isMultiSampleEnabled() const { return IsMultiSampleEnabled; }
	bool isSupportDepthTexture() const { return IsSupportDepthTexture; }
	bool isSupportA8L8() const { return IsSupportA8L8; }

public:
	virtual bool beginScene() = 0;
	virtual bool endScene() = 0;
	virtual bool clear(bool renderTarget, bool zBuffer, bool stencil, SColor color) = 0;

	virtual bool checkValid() = 0;
	virtual bool setRenderTarget(const IRenderTarget* texture, bool bindDepth = true) = 0;

	virtual void setTransform(E_TRANSFORMATION_STATE state, const matrix4& mat) = 0;
	virtual void setTexture(int index, ITexture* tex) = 0;

	virtual void setViewPort(const recti& area) = 0;
	virtual void setDisplayMode(const dimension2d& size) = 0;
	virtual bool setDriverSetting(const SDriverSetting& setting) = 0;

public:
	//
	virtual ITextureWriter* createTextureWriter(ITexture* texture) = 0;
	virtual bool removeTextureWriter(ITexture* texture) = 0;

	//
	virtual ITexture* createEmptyTexture(const dimension2d& size, ECOLOR_FORMAT format) = 0;
	virtual ITexture* getTextureWhite() const = 0;
	
	//
	virtual void add2DColor(const recti& rect, SColor color, E_2DBlendMode mode = E_Solid) = 0;
	virtual void add2DQuads(ITexture* texture, const SVertex_PCT* vertices, uint32_t numQuads, const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource()) = 0;
	virtual void flushAll2DQuads() = 0;
	virtual void draw2DImageBatch(ITexture* texture,
		const vector2di positions[],
		const recti* sourceRects[],
		uint32_t batchCount,
		SColor color,
		E_RECT_UVCOORDS uvcoords,
		const S2DBlendParam& blendParam) = 0;

public:
	void draw2DImage(ITexture* texture, const vector2di& pos,
		const recti* sourceRect = nullptr,
		SColor color = SColor::White(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource())
	{
		draw2DImageBatch(texture, &pos, sourceRect ? &sourceRect : nullptr, 1, color, uvcoords, blendParam);
	}
	
protected:
	void makeVPScaleMatrix(const recti& vpRect);

protected:
	const E_DRIVER_TYPE	DriverType;

	const SMaterial*	Material;
	const SGlobalMaterial*	GlobalMaterial;

	vector2df	OrthoCenterOffset;		//dx9有0.5像素的偏移

	const IRenderTarget*		CurrentRenderTarget;		//当前render target, 若为nullptr则表示frame buffer
	std::unique_ptr<IRenderTarget>		FrameBufferRT;

	matrix4		View2DTM;
	matrix4		Project2DTM;
	matrix4		VPScaleMatrix;
	matrix4		InvVPScaleMatrix;

	CAdapterInfo	AdapterInfo;

	recti			Viewport;
	dimension2d		ScreenSize;
	SDriverSetting	DriverSetting;

	bool	IsMultiSampleEnabled;
	bool	IsSupportDepthTexture;
	bool	IsSupportA8L8;

	uint32_t		PrimitivesDrawn;
	uint32_t		DrawCall;
};

inline void IVideoDriver::makeVPScaleMatrix(const recti& vpRect)
{
	VPScaleMatrix.makeIdentity();
	VPScaleMatrix._11 = (float)vpRect.getWidth() * 0.5f;
	VPScaleMatrix._22 = -(float)vpRect.getHeight() * 0.5f;
	VPScaleMatrix._33 = 1.0f;
	VPScaleMatrix._41 = /* (float)vpRect.left + */ (float)vpRect.getWidth() * 0.5f;
	VPScaleMatrix._42 = /* (float)vpRect.top + */ (float)vpRect.getHeight() * 0.5f;
	VPScaleMatrix._43 = 0.0f;

	InvVPScaleMatrix = VPScaleMatrix.getInverse();
}