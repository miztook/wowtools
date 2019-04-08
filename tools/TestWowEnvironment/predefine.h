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

#ifndef QMAX_PATH
#define		QMAX_PATH	512
#endif
