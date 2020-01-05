#pragma once

#include "predefine.h"
#include <stdint.h>
#include <memory>

#ifdef A_PLATFORM_WIN_DESKTOP

typedef		HWND				window_type;
typedef		HDC					dc_type;
typedef		HGLRC				glcontext_type;

#else

typedef		void*				HANDLE;
typedef		void*				window_type;
typedef		void*				dc_type;
typedef		void*				glcontext_type;

//	typedef	double	f64;
#endif

#define PTR_TO_INT32(x)		((int32_t)((intptr_t)(x) & 0xffffffff))
#define PTR_TO_UINT32(x)	((uint32_t)((uintptr_t)(x) & 0xffffffff))

#define INT32_TO_PTR(x)		((intptr_t)((intptr_t)((int32_t)x) & 0xffffffff))
#define UINT32_TO_PTR(x)	((uintptr_t)((uintptr_t)((uint32_t)x) & 0xffffffff))

#define  MATERIAL_MAX_TEXTURES		7

struct SWindowInfo
{
	SWindowInfo() : hwnd(nullptr), width(0), height(0) {}

	window_type hwnd;
	uint32_t width;
	uint32_t height;
};

struct SRendererInfo
{
	SRendererInfo() : adapter(0), vsync(true), antialias(1) {}

	uint32_t adapter;
	bool vsync;
	uint8_t  antialias;
};

class IMessageHandler
{
public:
	virtual void onExit(window_type hwnd) = 0;
	virtual void onSize(window_type hwnd, int width, int height) = 0;
};

enum E_INPUT_DEVICE : int32_t
{
	EID_KEYBOARD = 0,
	EID_MOUSE,
};

enum E_MOUSE_BUTTON : int32_t
{
	EMB_NONE = 0,
	EMB_LEFT = 1,
	EMB_RIGHT = 2,
	EMB_MIDDLE = 4,
};

enum E_MODIFIER_KEY : int32_t
{
	EMK_NONE = 0,
	EMK_SHIFT = 1,
	EMK_CTRL = 2,
	EMK_ALT = 4,
	EMK_LEFTMOUSE = 8,
	EMK_RIGHTMOUSE = 16,
	EMK_MIDDLEMOUSE = 32,
};

enum E_INPUT_MESSAGE : int32_t
{
	InputMessage_None = 0,
	Mouse_LeftButtonDown = 1,
	Mouse_LeftButtonUp,
	Mouse_RightButtonDown,
	Mouse_RightButtonUp,
	Mouse_Move,

	Key_Down = 100,
	Key_Up,
};

enum E_DRIVER_TYPE : int
{
	EDT_NULL = 0,
	EDT_OPENGL,
	EDT_COUNT,
};

enum E_AA_MODE : int
{
	E_AA_NONE = 0,
	E_AA_MSAA_1,
	E_AA_MSAA_2,
	E_AA_MSAA_3,
	E_AA_MSAA_4,
	E_AA_FXAA,
};

enum E_LIGHT_TYPE : int32_t
{
	ELT_POINT = 0,
	ELT_SPOT,
	ELT_DIRECTIONAL,
	ELT_AMBIENT,
};

enum E_VERTEX_TYPE : int32_t
{
	EVT_INVALID = -1,
	EVT_PC,			//for bounding box
	EVT_PCT,
	EVT_PNC,
	EVT_PNCT,
	EVT_PNCT2,
	EVT_PNT2WA,						//fvf
	EVT_COUNT,
};

enum E_INDEX_TYPE : int
{
	EIT_INVALID = -1,
	EIT_16BIT = 0,
	EIT_32BIT,
	EIT_COUNT,
};

enum ECOLOR_FORMAT : int
{
	ECF_UNKNOWN = 0,

	//8
	ECF_A8,
	//16
	ECF_A8L8,

	ECF_A1R5G5B5,			//argb in dx9 and abgr in gl and dx11
	ECF_R5G6B5,

	//24
	ECF_R8G8B8,

	//32
	//ECF_A8B8G8R8,
	ECF_A8R8G8B8,			//argb in dx9 and abgr in gl and dx11

	//float for RenderTarget Buffer
	ECF_ARGB32F,

	//DXT
	ECF_DXT1,
	ECF_DXT3,
	ECF_DXT5,

	//PVR
	ECF_PVRTC1_RGB_2BPP,
	ECF_PVRTC1_RGBA_2BPP,
	ECF_PVRTC1_RGB_4BPP,
	ECF_PVRTC1_RGBA_4BPP,

	//ETC
	ECF_ETC1_RGB,
	ECF_ETC1_RGBA,

	//ATC
	ECF_ATC_RGB,
	ECF_ATC_RGBA_EXPLICIT,
	ECF_ATC_RGBA_INTERPOLATED,

	//DEPTH
	ECF_D16,
	ECF_D24,
	ECF_D24S8,
	ECF_D32,
	ECF_INTZ,			//dx
};

struct STexFormatDesc
{
	ECOLOR_FORMAT format;
	uint32_t blockBytes;
	uint32_t blockWidth;
	uint32_t blockHeight;
	uint32_t minWidth;
	uint32_t minHeight;
	char text[32];
	bool hasAlpha;
};

static STexFormatDesc g_FormatDesc[] =
{
	{ ECF_UNKNOWN, 0, 0, 0, 0, 0, "UNKNOWN", false, },
	{ ECF_A8, 1, 1, 1, 1, 1, "A8", true, },
	{ ECF_A8L8, 2, 1, 1, 1, 1, "A8L8", true, },
	{ ECF_A1R5G5B5, 2, 1, 1, 1, 1, "A1R5G5B5", false, },
	{ ECF_R5G6B5, 2, 1, 1, 1, 1, "R5G6B5", false, },
	{ ECF_R8G8B8, 3, 1, 1, 1, 1, "R8G8B8", false, },
	{ ECF_A8R8G8B8, 4, 1, 1, 1, 1, "A8R8G8B8", true, },
	{ ECF_ARGB32F, 16, 1, 1, 1, 1, "ARGB32F", true },
	{ ECF_DXT1, 8, 4, 4, 4, 4, "DXT1", false, },
	{ ECF_DXT3, 16, 4, 4, 4, 4, "DXT3", true, },
	{ ECF_DXT5, 16, 4, 4, 4, 4, "DXT5", true, },
	{ ECF_PVRTC1_RGB_2BPP, 8, 8, 4, 16, 8, "PVRTC1_RGB_2BPP", false, },
	{ ECF_PVRTC1_RGBA_2BPP, 8, 8, 4, 16, 8, "PVRTC1_RGBA_2BPP", true, },
	{ ECF_PVRTC1_RGB_4BPP, 8, 4, 4, 8, 8, "PVRTC1_RGB_4BPP", false, },
	{ ECF_PVRTC1_RGBA_4BPP, 8, 4, 4, 8, 8, "PVRTC1_RGBA_4BPP", true, },
	{ ECF_ETC1_RGB, 8, 4, 4, 4, 4, "ETC1_RGB", false, },
	{ ECF_ETC1_RGBA, 8, 4, 4, 4, 4, "ETC1_RGBA", true, },
	{ ECF_ATC_RGB, 8, 4, 4, 4, 4, "ATC_RGB", false, },
	{ ECF_ATC_RGBA_EXPLICIT, 16, 4, 4, 4, 4, "ATC_RGBA_EXPLICIT", true, },
	{ ECF_ATC_RGBA_INTERPOLATED, 16, 4, 4, 4, 4, "ATC_RGBA_INTERPOLATED", true, },

	{ ECF_D16, 2, 1, 1, 1, 1, "DEPTH16", false, },
	{ ECF_D24, 4, 1, 1, 1, 1, "DEPTH24", false, },
	{ ECF_D24S8, 4, 1, 1, 1, 1, "DEPTH24STENCIL8", false, },
	{ ECF_D32, 4, 1, 1, 1, 1, "DEPTH32", false, },
	{ ECF_INTZ, 4, 1, 1, 1, 1, "DEPTHINTZ", false },
};

inline bool hasAlpha(ECOLOR_FORMAT format)
{
	ASSERT(static_cast<uint32_t>(format) < ARRAY_COUNT(g_FormatDesc));
	return g_FormatDesc[format].hasAlpha;
}

inline uint32_t getBytesPerPixelFromFormat(ECOLOR_FORMAT format)
{
	ASSERT(static_cast<uint32_t>(format) < ARRAY_COUNT(g_FormatDesc));
	return g_FormatDesc[format].blockBytes;
}

inline bool isCompressedFormat(ECOLOR_FORMAT format)
{
	ASSERT(static_cast<uint32_t>(format) < ARRAY_COUNT(g_FormatDesc));
	return g_FormatDesc[format].blockWidth > 1;
}

inline bool isCompressedWithAlphaFormat(ECOLOR_FORMAT format)
{
	return format == ECF_ETC1_RGBA;
}

inline const char* getColorFormatString(ECOLOR_FORMAT format)
{
	ASSERT(static_cast<uint32_t>(format) < ARRAY_COUNT(g_FormatDesc));
	return g_FormatDesc[format].text;
}

inline void getImageSize(ECOLOR_FORMAT format, uint32_t width, uint32_t height, uint32_t& w, uint32_t& h)
{
	ASSERT(static_cast<uint32_t>(format) < ARRAY_COUNT(g_FormatDesc));

	uint32_t bw = g_FormatDesc[format].blockWidth;
	uint32_t bh = g_FormatDesc[format].blockHeight;

	uint32_t mw = g_FormatDesc[format].minWidth;
	uint32_t mh = g_FormatDesc[format].minHeight;

	if (bw > 1)			//compressed
	{
		w = MAX(mw, (width + (bw - 1))) / bw;
		h = MAX(mh, (height + (bh - 1))) / bh;
	}
	else
	{
		w = width;
		h = height;
	}
}

inline void getImagePitchAndBytes(ECOLOR_FORMAT format, uint32_t width, uint32_t height, uint32_t& pitch, uint32_t& bytes)
{
	uint32_t bpp = getBytesPerPixelFromFormat(format);

	uint32_t w, h;
	getImageSize(format, width, height, w, h);

	pitch = w * bpp;
	bytes = pitch * h;
}

enum E_IMAGE_TYPE : int
{
	EIT_NONE = 0,
	EIT_IMAGE,
	EIT_BLP,
};

enum E_CULL_MODE : int
{
	ECM_NONE = 0,
	ECM_FRONT,
	ECM_BACK,
};

enum E_ANTI_ALIASING_MODE : int
{
	EAAM_OFF = 0,
	EAAM_SIMPLE,
	EAAM_LINE_SMOOTH,
};

enum E_COMPARISON_FUNC : int
{
	ECFN_NEVER = 0,
	ECFN_LESSEQUAL,
	ECFN_EQUAL,
	ECFN_LESS,
	ECFN_NOTEQUAL,
	ECFN_GREATEREQUAL,
	ECFN_GREATER,
	ECFN_ALWAYS,
};

enum E_COLOR_WRITE : uint8_t
{
	COLORWRITE_RED = 1,
	COLORWRITE_GREEN = 2,
	COLORWRITE_BLUE = 4,
	COLORWRITE_ALPHA = 8,
	COLORWRITE_ALL = 0xf,
};

enum E_BLEND_FACTOR : int
{
	EBF_ZERO = 0,
	EBF_ONE,
	EBF_DST_COLOR,
	EBF_ONE_MINUS_DST_COLOR,
	EBF_SRC_COLOR,
	EBF_ONE_MINUS_SRC_COLOR,
	EBF_SRC_ALPHA,
	EBF_ONE_MINUS_SRC_ALPHA,
	EBF_DST_ALPHA,
	EBF_ONE_MINUS_DST_ALPHA,
};

enum E_RECT_UVCOORDS : int
{
	ERU_00_11 = 0,
	ERU_01_10,
	ERU_10_01,
	ERU_11_00,
};

enum E_TEXTURE_FILTER : int
{
	ETF_NONE = 0,
	ETF_BILINEAR,
	ETF_TRILINEAR,
	ETF_ANISOTROPIC_X1,
	ETF_ANISOTROPIC_X2,
	ETF_ANISOTROPIC_X4,
	ETF_ANISOTROPIC_X8,
	ETF_ANISOTROPIC_X16,
};

inline uint8_t getAnisotropic(E_TEXTURE_FILTER filter)
{
	switch (filter)
	{
	case ETF_ANISOTROPIC_X1:
		return 1;
	case ETF_ANISOTROPIC_X2:
		return 2;
	case ETF_ANISOTROPIC_X4:
		return 4;
	case ETF_ANISOTROPIC_X8:
		return 8;
	case ETF_ANISOTROPIC_X16:
		return 16;
	default:
		return 1;
	}
}

enum E_TEXTURE_ADDRESS : int
{
	ETA_U = 0,
	ETA_V,
	ETA_COUNT,
};

enum E_TEXTURE_CLAMP : int
{
	ETC_REPEAT = 0,
	ETC_CLAMP,
	ETC_MIRROR,
};

enum E_MESHBUFFER_MAPPING : int
{
	EMM_SOFTWARE = 0,
	EMM_STATIC,
	EMM_DYNAMIC,
	EMM_COUNT,
};

enum E_PRIMITIVE_TYPE : int
{
	EPT_POINTS = 0,
	EPT_LINE_STRIP,
	EPT_LINES,
	EPT_TRIANGLE_STRIP,
	EPT_TRIANGLES,
	EPT_COUNT,
};

inline uint32_t getPrimitiveCount(E_PRIMITIVE_TYPE primType, uint32_t count)
{
	uint32_t p = 0;

	switch (primType)
	{
	case EPT_POINTS:
		p = count;
		break;
	case EPT_LINES:
		ASSERT(count >= 2);
		p = count / 2;
		break;
	case EPT_LINE_STRIP:
		ASSERT(count >= 2);
		p = count - 1;
		break;
	case EPT_TRIANGLE_STRIP:
		ASSERT(count >= 3);
		p = count - 2;
		break;
	case EPT_TRIANGLES:
		ASSERT(count >= 3);
		p = count / 3;
		break;
	default:
		ASSERT(false);
	}
	return p;
}

inline uint32_t getIndexCount(E_PRIMITIVE_TYPE primType, uint32_t primCount)
{
	uint32_t p = 0;

	switch (primType)
	{
	case EPT_POINTS:
		p = primCount;
		break;
	case EPT_LINES:
		p = primCount * 2;
		break;
	case EPT_LINE_STRIP:
		p = primCount + 1;
		break;
	case EPT_TRIANGLE_STRIP:
		p = primCount + 2;
		break;
	case EPT_TRIANGLES:
		p = primCount * 3;
		break;
	default:
		ASSERT(false);
	}
	return p;
}

enum E_UNIFORM_TYPE : int
{
	EUT_FLOAT = 0,
	EUT_VEC2,
	EUT_VEC3,
	EUT_VEC4,
	EUT_MAT2,
	EUT_MAT3,
	EUT_MAT4,
	EUT_SAMPLER1D,
	EUT_SAMPLER2D,
	EUT_SAMPLER3D,

	EUT_COUNT,
};

enum E_LAYER : int
{
	LAYER_DEFAULT = 0,


};

enum E_VIDEO_DRIVER_FEATURE : int
{
	EVDF_RENDER_TO_TARGET = 0,
	EVDF_HARDWARE_TL,
	EVDF_TEXTURE_ADDRESS,
	EVDF_SEPARATE_UVWRAP,
	EVDF_MIP_MAP,
	EVDF_STENCIL_BUFFER,
	EVDF_VERTEX_SHADER_2_0,
	EVDF_VERTEX_SHADER_3_0,
	EVDF_PIXEL_SHADER_2_0,
	EVDF_PIXEL_SHADER_3_0,
	EVDF_TEXTURE_NSQUARE,
	EVDF_TEXTURE_NPOT,
	EVDF_COLOR_MASK,
	EVDF_MULTIPLE_RENDER_TARGETS,
	EVDF_MRT_COLOR_MASK,
	EVDF_MRT_BLEND_FUNC,
	EVDF_MRT_BLEND,
	EVDF_STREAM_OFFSET,
	EVDF_W_BUFFER,

	//! Supports Shader model 4
	EVDF_VERTEX_SHADER_4_0,
	EVDF_PIXEL_SHADER_4_0,
	EVDF_GEOMETRY_SHADER_4_0,
	EVDF_STREAM_OUTPUT_4_0,
	EVDF_COMPUTING_SHADER_4_0,

	//! Supports Shader model 4.1
	EVDF_VERTEX_SHADER_4_1,
	EVDF_PIXEL_SHADER_4_1,
	EVDF_GEOMETRY_SHADER_4_1,
	EVDF_STREAM_OUTPUT_4_1,
	EVDF_COMPUTING_SHADER_4_1,

	//! Supports Shader model 5.0
	EVDF_VERTEX_SHADER_5_0,
	EVDF_PIXEL_SHADER_5_0,
	EVDF_GEOMETRY_SHADER_5_0,
	EVDF_STREAM_OUTPUT_5_0,
	EVDF_TESSELATION_SHADER_5_0,
	EVDF_COMPUTING_SHADER_5_0,

	//! Supports texture multisampling
	EVDF_TEXTURE_MULTISAMPLING,
	EVDF_COUNT,
};