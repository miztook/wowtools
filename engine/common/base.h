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
