#pragma once

#include "predefine.h"
#include <stdint.h>

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

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&);               \
TypeName& operator=(const TypeName&)


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
	EVT_P = 0,
	EVT_PC,			//for bounding box
	EVT_PCT,
	EVT_PN,
	EVT_PNC,
	EVT_PNT,
	EVT_PT,
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

enum E_IMAGE_TYPE : int
{
	EIT_NONE = 0,
	EIT_IMAGE,
	EIT_DDS,
};

enum E_TRANSFORMATION_STATE : int
{
	ETS_VIEW = 0,
	ETS_WORLD,
	ETS_PROJECTION,
	ETS_TEXTURE_0,
	ETS_TEXTURE_1,
	ETS_TEXTURE_2,
	ETS_TEXTURE_3,
	ETS_COUNT,
};