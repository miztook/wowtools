#pragma once

#include "base.h"
#include "rect.h"
#include "vector2d.h"
#include "matrix4.h"
#include "SMaterial.h"
#include <string>
#include <list>

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
	}

	bool vsync;
	E_AA_MODE aaMode;
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

protected:
	void makeVPScaleMatrix(const recti& vpRect);

protected:
	const E_DRIVER_TYPE	DriverType;

	SMaterial	Material;
	SGlobalMaterial		GlobalMaterial;

	vector2df	OrthoCenterOffset;		//dx9ÓÐ0.5ÏñËØµÄÆ«ÒÆ
	SMaterial	InitMaterial2D;
	SGlobalMaterial		InitGlobalMaterial2D;

	matrix4		WVP;
	matrix4		WV;
	matrix4		Matrices[ETS_COUNT];
	matrix4		View2DTM;
	matrix4		Project2DTM;
	matrix4		VPScaleMatrix;
	matrix4		InvVPScaleMatrix;

	recti			Viewport;
	vector2df		ScreenSize;
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