#pragma once

#include "base.h"
#include "rect.h"
#include "vector2d.h"
#include "matrix4.h"
#include "SMaterial.h"
#include "IRenderTarget.h"
#include <string>
#include <list>

class ITexture;
class ITextureWriter;

enum E_RENDER_MODE : int32_t
{
	ERM_NONE = 0,
	ERM_2D,
	ERM_3D,
	ERM_STENCIL_FILL,
};

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
	CAdapterInfo() : index(0), vendorID(0)
	{
	}

public:
	uint32_t		index;
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
		InitMaterial2D.RasterizerDesc.Cull = ECM_BACK;
		InitMaterial2D.Lighting = false;
		InitMaterial2D.DepthStencilDesc.ZWriteEnable = false;
		InitMaterial2D.DepthStencilDesc.ZBuffer = ECFN_NEVER;
		InitGlobalMaterial2D.TextureFilter = ETF_BILINEAR;
	}

	virtual ~IVideoDriver() {}

public:
	E_DRIVER_TYPE getDriverType() const { return DriverType; }

	const vector2df& getOrthoCenterOffset() const { return OrthoCenterOffset; }
	SGlobalMaterial& getOverrideMaterial() { return GlobalMaterial; }
	const matrix4& getTransform(E_TRANSFORMATION_STATE state) const { return Matrices[state]; }
	const recti& getViewPort() const { return Viewport; }
	const dimension2d& getDisplayMode() const { return ScreenSize; }
	const SDriverSetting& getDriverSetting() const { return DriverSetting; }

	void setMaterial(const SMaterial& material) { Material = material; }
	const SMaterial& getMaterial() const { return Material; }

	const matrix4& getWVPMatrix() const { return WVP; }
	const matrix4& getWVMatrix() const { return WV; }
	const matrix4& getWMatrix() const { return getTransform(ETS_WORLD); }
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

	//
	virtual ITextureWriter* createTextureWriter(ITexture* texture) = 0;
	virtual bool removeTextureWriter(ITexture* texture) = 0;

protected:
	void makeVPScaleMatrix(const recti& vpRect);

protected:
	const E_DRIVER_TYPE	DriverType;

	SMaterial	Material;
	SGlobalMaterial		GlobalMaterial;

	vector2df	OrthoCenterOffset;		//dx9有0.5像素的偏移
	SMaterial	InitMaterial2D;
	SGlobalMaterial		InitGlobalMaterial2D;

	const IRenderTarget*		CurrentRenderTarget;		//当前render target, 若为nullptr则表示frame buffer
	std::unique_ptr<IRenderTarget>		FrameBufferRT;

	matrix4		WVP;
	matrix4		WV;
	matrix4		Matrices[ETS_COUNT];
	matrix4		View2DTM;
	matrix4		Project2DTM;
	matrix4		VPScaleMatrix;
	matrix4		InvVPScaleMatrix;

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