#pragma once

//Platform Defines
#if defined(_MSC_VER)

#define A_PLATFORM_WIN_DESKTOP 1

#elif defined(__ANDROID__)
//Attention: "__linux__" is also defined on Android platform.
#define A_PLATFORM_ANDROID 1

#elif defined(__APPLE__)

#define A_PLATFORM_XOS 1

#elif defined(__linux__) || defined(LINUX)

#define A_PLATFORM_LINUX 1

#else

#pragma error("unknown platform!")

#endif

#if A_PLATFORM_WIN_DESKTOP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <malloc.h>

#pragma warning(disable : 4251)
#pragma warning(disable : 4355) // this used in base initializer list
#pragma warning(disable : 4996)
#pragma warning(disable : 4481)

#else

#include <alloca.h>

#endif
#include <cassert>

#ifndef QMAX_PATH
#define		QMAX_PATH	512
#endif

#ifndef DELETE_ARRAY
#define DELETE_ARRAY(T, p)		{ delete[] (static_cast<const T*>(p)); }
#endif

#define ROUND_N_BYTES(x, n) ((x+(n-1)) & ~(n-1))
#define ROUND_4BYTES(x) ((x+3) & ~3)
#define ROUND_8BYTES(x) ((x+7) & ~7)
#define ROUND_16BYTES(x) ((x+15) & ~15)
#define ROUND_32BYTES(x) ((x+31) & ~31)

#if defined(_DEBUG) || defined(DEBUG)
#ifndef ASSERT
#define ASSERT	assert
#endif	//	ASSERT
#else
#undef ASSERT
#define ASSERT(x)
#endif

#ifndef ARRAY_COUNT
#define ARRAY_COUNT(a)		(sizeof(a)/sizeof(*a))
#endif

#ifndef MAX
#define MAX(a,b) (a < b ? b : a)
#endif

#ifndef FOURCC
#define FOURCC(c0, c1, c2, c3) (c0 | (c1 << 8) | (c2 << 16) | (c3 << 24))
#endif

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
TypeName(const TypeName&) = delete;               \
TypeName& operator=(const TypeName&) = delete;

#define  MATERIAL_MAX_TEXTURES		7