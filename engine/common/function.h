#pragma once

#include "predefine.h"
#include <cmath>
#include <cfloat>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cerrno>
#include <cctype>
#include <ctime>
#include <functional>

#ifdef A_PLATFORM_WIN_DESKTOP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <locale.h>
#include <share.h>
#include <direct.h>
#include <io.h>
#include <malloc.h>
#else
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#endif

//
//		math
//

#define IR(x)                           ((unsigned int&)(x))
#define FR(x)                           ((float&)(x))

const float ROUNDING_ERROR_f32 = 0.000001f;
const float PI = 3.14159265358979324f;

const float DEGTORAD = 0.01745329251994329577f;
const float RADTODEG = 57.2957795130823208768f;

inline float radToDeg(float radians) { return RADTODEG*radians; }
inline float degToRad(float degrees) { return DEGTORAD*degrees; }

inline bool iszero_(const float a, const float tolerance = 0.000001f)
{
	return fabsf(a) <= tolerance;
}

template<class T>
inline const T& min_(const T& a, const T& b)
{
	return a < b ? a : b;
}
template<class T>
inline const T& max_(const T& a, const T& b)
{
	return a < b ? b : a;
}
template<class T>
inline T abs_(const T& a)
{
	return a < (T)0.f ? -a : a;
}
template<class T>
inline T lerp_(const T& a, const T& b, const float t)
{
	return (T)(a*(1.f - t)) + (b*t);
}
template <class T>
inline const T clamp_(const T& value, const T& low, const T& high)
{
	return min_(max_(value, low), high);
}
template <class T>
inline void swap_(T& a, T& b)
{
	T c(a);
	a = b;
	b = c;
}

inline bool equals_(const float a, const float b, float tolerance = ROUNDING_ERROR_f32)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}
inline bool equals_(const int32_t a, const int32_t b, int32_t tolerance = 0)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}
inline bool equals_(const uint32_t a, const uint32_t b, int32_t tolerance = 0)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}

inline float squareroot_(const float f)
{
	return sqrtf(f);
}

inline float reciprocal_squareroot_(const float f)
{
	if (iszero_(f))
	{
		ASSERT(false);
		return FLT_MAX;
	}
	else
		return 1.f / sqrtf(f);
}

inline float reciprocal_(const float f)
{
	if (iszero_(f))
	{
		ASSERT(false);
		return FLT_MAX;
	}
	else
		return 1.f / f;
}
inline int floor32_(float x)
{
	return (int)floorf(x);
}
inline int ceil32_(float x)
{
	return (int)ceilf(x);
}
inline float round_(float x)
{
	return floorf(x + 0.5f);
}
inline int round32_(float x)
{
	return (int)round_(x);
}
inline  float fract_(float x)
{
	return x - floorf(x);
}

inline bool isPOT(unsigned int x)
{
	return (x > 0) && ((x & (x - 1)) == 0);
}

inline void flipcc(char fcc[4])
{
	char t;
	t = fcc[0];
	fcc[0] = fcc[3];
	fcc[3] = t;
	t = fcc[1];
	fcc[1] = fcc[2];
	fcc[2] = t;
}

inline void Q_memcpy(void * dst,
	size_t bytes,
	const void * src,
	size_t count)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	memcpy_s(dst, bytes, src, count);
#else
	memcpy(dst, src, count);
#endif
}

inline void Q_memset(void *dst,
	int val,
	size_t count)
{
	memset(dst, val, count);
}

inline int Q_memcmp(const void * buf1,
	const void * buf2,
	size_t count)
{
	return memcmp(buf1, buf2, count);
}

#ifdef A_PLATFORM_WIN_DESKTOP
#define Q_sscanf		sscanf_s
#else
#define Q_sscanf		sscanf
#endif

#ifdef A_PLATFORM_WIN_DESKTOP
#define Q_swprintf swprintf_s
#else
#define Q_swprintf swprintf
#endif

#ifdef A_PLATFORM_WIN_DESKTOP
#define Q_vsprintf vsprintf_s
#else
#define Q_vsprintf vsnprintf
#endif

inline int64_t Q_atoi64(const char * szString)
{
	return (int64_t)atoll(szString);
}

inline void Q_strcpy(char * dst, size_t count, const char * src)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	strcpy_s(dst, count, src);
#else
	strncpy(dst, src, count - 1);
	dst[count - 1] = '\0';
#endif
}

inline void Q_strlwr(char * dst)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	_strlwr_s(dst, strlen(dst) + 1);
#else
	char* p = dst;
	for (; *p != 0; p++)
		*p = tolower(*p);
#endif
}

inline void Q_strupr(char * dst)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	_strupr_s(dst, strlen(dst) + 1);
#else
	char* p = dst;
	for (; *p != 0; p++)
		*p = toupper(*p);
#endif
}

inline void Q_strncpy(char * dest,
	size_t bytes,
	const char * source,
	size_t count)
{
	ASSERT(bytes >= count + 1);
#ifdef A_PLATFORM_WIN_DESKTOP
	strncpy_s(dest, bytes, source, count);
#else
	strncpy(dest, source, count);
#endif
}

inline void Q_strcat(char * dst, size_t count, const char * src)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	strcat_s(dst, count, src);
#else
	strcat(dst, src);
#endif
}

inline int Q_strcmp(const char * src,
	const char * dst)
{
	return strcmp(src, dst);
}

inline int Q_stricmp(const char * src,
	const char * dst)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	return _stricmp(src, dst);
#else
	return strcasecmp(src, dst);
#endif
}

inline int Q_strnicmp(const char * src, const char * dst, size_t len)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	return _strnicmp(src, dst, len);
#else
	return strncasecmp(src, dst, len);
#endif
}

inline size_t Q_strlen(const char * src)
{
	return strlen(src);
}

inline char* Q_strtok(char* str, const char* delim, char** saveptr)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	return strtok_s(str, delim, saveptr);
#else
	return strtok_r(str, delim, saveptr);
#endif
}

inline int Q_sprintf(char *string, size_t sizeInBytes, const char *format, ...)
{
	va_list arglist;
	va_start(arglist, format);

	int ret = Q_vsprintf(string, sizeInBytes, format, arglist);

	va_end(arglist);

	return ret;
}

inline FILE*  Q_fopen(const char * filename, const char * mode)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	return _fsopen(filename, mode, _SH_DENYWR);
#else
	return fopen(filename, mode);
#endif
}

inline int Q_access(const char * filename, int mode)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	return (int)_access_s(filename, mode);
#else
	return access(filename, mode);
#endif
}

#ifdef A_PLATFORM_WIN_DESKTOP
#define Q_snprintf	_snprintf
#else
#define Q_snprintf	snprintf
#endif

#define Q_isnan   std::isnan

inline void Q_getLocalTime(char* timebuf, size_t size)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	SYSTEMTIME time;
	GetLocalTime(&time);
	Q_sprintf(timebuf, size, "%0.2d/%0.2d %0.2d:%0.2d:%0.2d.%0.3d\t", time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
#else
	::tm t;
	::time_t m_time;
	::timeval timeofday;
	::localtime_r(&m_time, &t);
	::gettimeofday(&timeofday, NULL);
	Q_sprintf(
		timebuf,
		size,
		"%0.2d/%0.2d %0.2d:%0.2d:%0.2d.%0.3d\t",
		t.tm_mon + 1,
		t.tm_mday,
		t.tm_hour,
		t.tm_min,
		t.tm_sec,
		timeofday.tv_usec / 1000);
#endif
}

inline void Q_getLocalDate(char* timebuf, size_t size)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	SYSTEMTIME time;
	GetLocalTime(&time);
	Q_sprintf(timebuf, size, "%0.4d-%0.2d-%0.2d", time.wYear, time.wMonth, time.wDay);
#else
	::tm t;
	::time_t m_time;
	::timeval timeofday;
	::localtime_r(&m_time, &t);
	::gettimeofday(&timeofday, NULL);
	Q_sprintf(
		timebuf,
		size,
		"%0.4d-%0.2d-%0.2d",
		t.tm_year + 1900,
		t.tm_mon + 1,
		t.tm_mday);
#endif
}

inline void Q_getLocalDateTimeMs(char* timebuf, size_t size)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	SYSTEMTIME time;
	GetLocalTime(&time);
	Q_sprintf(timebuf, size, "%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d.%0.3d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
#else
	::tm t;
	::time_t m_time;
	::timeval timeofday;
	::localtime_r(&m_time, &t);
	::gettimeofday(&timeofday, NULL);
	Q_sprintf(
		timebuf,
		size,
		"%0.4d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d.%0.3d",
		t.tm_year + 1900,
		t.tm_mon + 1,
		t.tm_mday,
		t.tm_hour,
		t.tm_min,
		t.tm_sec,
		timeofday.tv_usec / 1000);
#endif
}

inline void Q_getLocalDateTimeMinute(char* timebuf, size_t size)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	SYSTEMTIME time;
	GetLocalTime(&time);
	Q_sprintf(timebuf, size, "%0.4d-%0.2d-%0.2d_%0.2d_%0.2d", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
#else
	::tm t;
	::time_t m_time;
	::timeval timeofday;
	::localtime_r(&m_time, &t);
	::gettimeofday(&timeofday, NULL);
	Q_sprintf(
		timebuf,
		size,
		"%0.4d-%0.2d-%0.2d_%0.2d_%0.2d",
		t.tm_year + 1900,
		t.tm_mon + 1,
		t.tm_mday,
		t.tm_hour,
		t.tm_min);
#endif
}

inline void Q_fullpath(const char* filename, char* outfilename, size_t size)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	if (!_fullpath(outfilename, filename, size))
	{
		ASSERT(false);
	}
#else
	if (!realpath(filename, outfilename))
		strcpy(outfilename, filename);
#endif
}

inline int Q_mkdir(const char* path)
{
#ifdef A_PLATFORM_WIN_DESKTOP
	return mkdir(path);
#else
	return mkdir(path, 0777);
#endif
}

inline int Q_rmdir(const char* path)
{
	return rmdir(path);
}

inline char* Q_getcwd(char* dstbuf, int sizebytes)
{
	return getcwd(dstbuf, sizebytes);
}

inline bool isAbsoluteFileName(const char* filename)
{
	unsigned int len = (unsigned int)strlen(filename);
#ifdef  A_PLATFORM_WIN_DESKTOP
	if (len >= 2)
		return filename[1] == ':';
#else
	if (len >= 1)
		return filename[0] == '/';
#endif

	return false;
}

inline void normalizeFileName(const char* filename, char* outfilename, unsigned int size)
{
	ASSERT(size >= strlen(filename) + 1);
	unsigned int len = (unsigned int)strlen(filename);
	for (unsigned int i = 0; i < len; ++i)
	{
		if (filename[i] == '\\')
			outfilename[i] = '/';
		else
			outfilename[i] = filename[i];
	}
	outfilename[len] = '\0';
}

inline void normalizeDirName(char* dirName)
{
	unsigned int len = (unsigned int)strlen(dirName);
	for (unsigned int i = 0; i < len; ++i)
	{
		if (dirName[i] == '\\')
			dirName[i] = '/';
	}

	if (len > 0)
	{
		char last = dirName[len - 1];
		if (last != '/' && last != '\\')
			Q_strcat(dirName, len + 2, "/");
	}
}

inline void normalizeFileName(char* filename)
{
	unsigned int len = (unsigned int)strlen(filename);
	for (unsigned int i = 0; i < len; ++i)
	{
		if (filename[i] == '\\')
			filename[i] = '/';
	}
}

inline bool isNormalized(const char* filename)
{
	unsigned int len = (unsigned int)strlen(filename);
	for (unsigned int i = 0; i < len; ++i)
	{
		if (filename[i] == '\\')
			return false;
	}
	return true;
}

inline bool isLowerFileName(const char* filename)
{
	unsigned int len = (unsigned int)strlen(filename);
	for (unsigned int i = 0; i < len; ++i)
	{
		if (isupper((unsigned char)filename[i]))
			return false;
	}
	return true;
}

using ITERATEFILECALLBACK = std::function<void(const char* filename)>;

inline bool Q_iterateFiles(const char* dirname, const char* ext, bool recursive, const char* initdir, ITERATEFILECALLBACK callback)
{
	if (!initdir)
		initdir = "";

	char path[QMAX_PATH];
	Q_strcpy(path, QMAX_PATH, dirname);
	normalizeDirName(path);
	Q_strcat(path, QMAX_PATH, ext);

	char initdirname[QMAX_PATH];
	Q_strcpy(initdirname, QMAX_PATH, initdir);
	normalizeDirName(initdirname);

	const bool bEmptyInit = strlen(initdirname) == 0;
	unsigned int len = (unsigned int)strlen(initdir);
	if (!bEmptyInit && initdirname[len - 1] != '\\' && initdirname[len - 1] != '/')
		++len;

#ifdef A_PLATFORM_WIN_DESKTOP
	_finddata_t finddata;
	intptr_t hfile = _findfirst(path, &finddata);
	if (hfile == -1)
		return false;

	do
	{
		if (strcmp(finddata.name, "..") == 0 || strcmp(finddata.name, ".") == 0)
			continue;

		if (finddata.attrib & (_A_HIDDEN | _A_SYSTEM))
			continue;

		char subpath[QMAX_PATH];
		Q_strcpy(subpath, QMAX_PATH, dirname);
		normalizeDirName(subpath);
		Q_strcat(subpath, QMAX_PATH, finddata.name);

		if (finddata.attrib & _A_SUBDIR)
		{
			if (recursive)
				Q_iterateFiles(subpath, ext, recursive, initdir, callback);
		}
		else
		{
			if (bEmptyInit)
			{
				callback(subpath);
			}
			else
			{
				char shortpath[QMAX_PATH];
				const char* start = strstr(subpath, initdirname);
				Q_strcpy(shortpath, QMAX_PATH, start ? start + len : subpath);
				callback(shortpath);
			}
		}
	} while (_findnext(hfile, &finddata) != -1);

	if (errno != ENOENT)
	{
		_findclose(hfile);
		return false;
	}

	_findclose(hfile);
	return true;

#else
	DIR* handle = opendir(path);
	if (NULL == handle)
	{
		perror("directory::open");
		return false;
	}

	while (dirent* pdata = readdir(handle))
	{
		if (strcmp(pdata->d_name, "..") == 0 || strcmp(pdata->d_name, ".") == 0)
			continue;

		if (pdata->d_name[0] == 0)
			continue;

		char subpath[QMAX_PATH];
		Q_strcpy(subpath, QMAX_PATH, dirname);
		normalizeDirName(subpath);
		Q_strcat(subpath, QMAX_PATH, pdata->d_name);

		//获取file state
		struct stat buf;
		if (0 != stat(subpath, &buf))
		{
			perror("stat() failed");
			closedir(handle);
			return false;
		}

		if (S_ISDIR(buf.st_mode))
		{
			if (recursive)
				Q_iterateFiles(subpath, ext, recusive, initdir, callback);
		}
		else
		{
			if (bEmptyInit)
			{
				callback(subpath);
			}
			else
			{
				char shortpath[QMAX_PATH];
				const char* start = strstr(subpath, initdirname);
				Q_strcpy(shortpath, QMAX_PATH, start ? start + len : subpath);
				callback(shortpath);
			}
		}
	}

	closedir(handle);
	return true;

#endif
}

inline bool Q_iterateFiles(const char* dirname, bool recursive, const char* initdir, ITERATEFILECALLBACK callback)
{
#if A_PLATFORM_WIN_DESKTOP
	return Q_iterateFiles(dirname, "*", recursive, initdir, callback);
#else
	return Q_iterateFiles(dirname, "", recursive, initdir, callback);
#endif
}


inline void getFileDirA(const char* filename, char* outfilename, unsigned int size)
{
	const char* lastSlash = strrchr(filename, ('/'));
	const char* lastBackSlash = strrchr(filename, ('\\'));
	if (!lastSlash || lastSlash < lastBackSlash)
		lastSlash = lastBackSlash;

	if (!lastSlash || lastSlash - filename <= 0)
		Q_strcpy(outfilename, size, ("."));
	else
	{
		Q_strncpy(outfilename, size, filename, lastSlash - filename);
		outfilename[lastSlash - filename] = '\0';
	}
}

inline void getFileNameA(const char* filename, char* outfilename, unsigned int size)
{
	const char* lastSlash = strrchr(filename, '/');
	const char* lastBackSlash = strrchr(filename, '\\');
	if (lastSlash < lastBackSlash)
		lastSlash = lastBackSlash;

	bool noPrefix = (!lastSlash || *(lastSlash + 1) == '\0');
	if (noPrefix)
		Q_strcpy(outfilename, size, filename);
	else
		Q_strcpy(outfilename, size, lastSlash + 1);
}

inline void getFileNameNoExtensionA(const char* filename, char* outfilename, unsigned int size)
{
	const char* lastSlash = strrchr(filename, '/');
	const char* lastBackSlash = strrchr(filename, '\\');
	if (lastSlash < lastBackSlash)
		lastSlash = lastBackSlash;
	const char* p = strrchr(filename, '.');
	if (p && p < lastSlash)
	{
		Q_strcpy(outfilename, size, filename);
		return;
	}

	bool noPrefix = (!lastSlash || *(lastSlash + 1) == '\0');
	bool noSuffix = (!p || *(p + 1) == '\0');
	if (noPrefix && noSuffix)
		Q_strcpy(outfilename, size, filename);
	else if (noPrefix)			//suffix
	{
		Q_strncpy(outfilename, size, filename, p - filename);
		outfilename[p - filename] = '\0';
	}
	else if (noSuffix)			//prefix
		Q_strcpy(outfilename, size, lastSlash + 1);
	else
	{
		Q_strncpy(outfilename, size, lastSlash + 1, p - (lastSlash + 1));
		outfilename[p - (lastSlash + 1)] = '\0';
	}
}

inline void getFileExtensionA(const char* filename, char* outfilename, unsigned int size)
{
	const char* lastSlash = strrchr(filename, '/');
	const char* lastBackSlash = strrchr(filename, '\\');
	if (lastSlash < lastBackSlash)
		lastSlash = lastBackSlash;

	const char* p = strrchr(filename, '.');
	if (p && p < lastSlash)
	{
		Q_strcpy(outfilename, size, "");
		return;
	}

	if (!p || *(p + 1) == '\0')
		Q_strcpy(outfilename, size, "");
	else
		Q_strcpy(outfilename, size, p + 1);
}

inline void getFullFileNameNoExtensionA(const char* filename, char* outfilename, unsigned int size)
{
	const char* lastSlash = strrchr(filename, '/');
	const char* lastBackSlash = strrchr(filename, '\\');
	if (lastSlash < lastBackSlash)
		lastSlash = lastBackSlash;

	const char* p = strrchr(filename, '.');
	if (p && p < lastSlash)
	{
		Q_strcpy(outfilename, size, "");
		return;
	}

	if (!p || *(p + 1) == '\0')
		Q_strcpy(outfilename, size, filename);
	else
	{
		Q_strncpy(outfilename, size, filename, p - filename);
		outfilename[p - filename] = '\0';
	}
}

inline bool hasFileExtensionA(const char* filename, const char* ext)
{
	if (*ext == '*')
		return true;

	char extension[32];
	getFileExtensionA(filename, extension, 32);

	if (Q_stricmp(extension, ext) != 0)
		return false;
	return true;
}

inline void makeHDFileName(char* name, unsigned int size, const char* filename)
{
	const char* p = strrchr(filename, '.');
	if (p)
	{
		Q_strncpy(name, size, filename, p - filename);
		Q_strcat(name, size, "_HD");
		Q_strcat(name, size, p);
	}
	else
	{
		Q_strcat(name, size, "_HD");
	}
}

inline bool Q_MakeDirForFileName(const char* filename)
{
	//create directory if not exist
	char dir[QMAX_PATH];
	getFileDirA(filename, dir, QMAX_PATH);
	normalizeFileName(dir);
	Q_strcat(dir, QMAX_PATH, "/");

	const char* p = strchr(dir, '/');

	while (p)
	{
		char tmp[QMAX_PATH];
		Q_strncpy(tmp, QMAX_PATH, dir, p - dir + 1);
		tmp[p - dir + 1] = '\0';
		if (Q_access(tmp, 0) != 0)
		{
			int ret = Q_mkdir(tmp);
			if (ret != 0 && ret != EEXIST)
				return false;
		}
		p = strchr(p + 1, '/');
	}
	return true;
}

inline void trim(const char* str, char* outstring, unsigned int size)
{
	unsigned int len = (unsigned int)strlen(str);
	if (len == 0)
	{
		Q_strcpy(outstring, size, "");
		return;
	}
	unsigned int left = 0;
	unsigned int right = len;
	for (unsigned int i = 0; i < len; ++i)
	{
		if (str[i] == ' ' || str[i] == '\n')
			++left;
		else
			break;
	}

	for (int i = (int)len - 1; i >= 0; --i)
	{
		if (str[i] == ' ' || str[i] == '\n')
			--right;
		else
			break;
	}

	if (left >= right)
	{
		Q_strcpy(outstring, size, "");
	}
	else
	{
		unsigned int copysize = min_(size - 1, right - left);
		Q_strncpy(outstring, size, &str[left], copysize);
		outstring[copysize] = '\0';
	}
}

inline void trim(const char* str, unsigned int count, char* outstring, unsigned int size)
{
	unsigned int len = count;
	if (len == 0)
	{
		Q_strcpy(outstring, size, "");
		return;
	}
	unsigned int left = 0;
	unsigned int right = len;
	for (unsigned int i = 0; i < len; ++i)
	{
		if (str[i] == ' ' || str[i] == '\n')
			++left;
		else
			break;
	}

	for (int i = (int)len - 1; i >= 0; --i)
	{
		if (str[i] == ' ' || str[i] == '\n')
			--right;
		else
			break;
	}

	if (left >= right)
	{
		Q_strcpy(outstring, size, "");
	}
	else
	{
		unsigned int copysize = min_(size - 1, right - left);
		Q_strncpy(outstring, size, &str[left], copysize);
		outstring[copysize] = '\0';
	}
}

// we write [17] here instead of [] to work around a swig bug
const float fast_atof_table[17] = {
	0.f,
	0.1f,
	0.01f,
	0.001f,
	0.0001f,
	0.00001f,
	0.000001f,
	0.0000001f,
	0.00000001f,
	0.000000001f,
	0.0000000001f,
	0.00000000001f,
	0.000000000001f,
	0.0000000000001f,
	0.00000000000001f,
	0.000000000000001f,
	0.0000000000000001f
};

//! Convert a simple string of base 10 digits into a signed 32 bit integer.
//! \param[in] in: The string of digits to convert. Only a leading - or + followed
//!					by digits 0 to 9 will be considered.  Parsing stops at the
//!					first non-digit.
//! \param[out] out: (optional) If provided, it will be set to point at the first
//!					 character not used in the calculation.
//! \return The signed integer value of the digits. If the string specifies too many
//!			digits to encode in an int then +INT_MAX or -INT_MAX will be returned.
inline int strtol10(const char* in, const char** out = 0)
{
	if (!in)
		return 0;

	bool negative = false;
	if ('-' == *in)
	{
		negative = true;
		++in;
	}
	else if ('+' == *in)
		++in;

	unsigned int unsignedValue = 0;

	while ((*in >= '0') && (*in <= '9'))
	{
		unsignedValue = (unsignedValue * 10) + (*in - '0');
		++in;

		if (unsignedValue > (unsigned int)INT_MAX)
		{
			unsignedValue = (unsigned int)INT_MAX;
			break;
		}
	}
	if (out)
		*out = in;

	if (negative)
		return -((int)unsignedValue);
	else
		return (int)unsignedValue;
}

//! Converts a sequence of digits into a whole positive floating point value.
//! Only digits 0 to 9 are parsed.  Parsing stops at any other character,
//! including sign characters or a decimal point.
//! \param in: the sequence of digits to convert.
//! \param out: (optional) will be set to point at the first non-converted character.
//! \return The whole positive floating point representation of the digit sequence.
inline float strtof10(const char* in, const char * * out = 0)
{
	if (out)
		*out = in;

	if (!in)
		return 0.f;

	static const unsigned int MAX_SAFE_U32_VALUE = UINT_MAX / 10 - 10;
	float floatValue = 0.f;
	unsigned int intValue = 0;

	// Use integer arithmetic for as long as possible, for speed
	// and precision.
	while ((*in >= '0') && (*in <= '9'))
	{
		// If it looks like we're going to overflow, bail out
		// now and start using floating point.
		if (intValue >= MAX_SAFE_U32_VALUE)
			break;

		intValue = (intValue * 10) + (*in - '0');
		++in;
	}

	floatValue = (float)intValue;

	// If there are any digits left to parse, then we need to use
	// floating point arithmetic from here.
	while ((*in >= '0') && (*in <= '9'))
	{
		floatValue = (floatValue * 10.f) + (float)(*in - '0');
		++in;
		if (floatValue > FLT_MAX) // Just give up.
			break;
	}

	if (out)
		*out = in;

	return floatValue;
}

//! Provides a fast function for converting a string into a float.
//! This is not guaranteed to be as accurate as atof(), but is
//! approximately 6 to 8 times as fast.
//! \param[in] in: The string to convert.
//! \param[out] out: The resultant float will be written here.
//! \return A pointer to the first character in the string that wasn't
//!         use to create the float value.
inline const char* fast_atof_move(const char * in, float & out)
{
	out = 0.f;
	if (!in)
		return 0;

	bool negative = false;
	if (*in == '-')
	{
		negative = true;
		++in;
	}

	float value = strtof10(in, &in);

	if (*in == '.')
	{
		++in;

		const char * afterDecimal = in;
		float decimal = strtof10(in, &afterDecimal);
		decimal *= fast_atof_table[afterDecimal - in];

		value += decimal;

		in = afterDecimal;
	}

	if ('e' == *in || 'E' == *in)
	{
		++in;
		// Assume that the exponent is a whole number.
		// strtol10() will deal with both + and - signs,
		// but cast to (float) to prevent overflow at FLT_MAX
		value *= (float)pow(10.0f, (float)strtol10(in, &in));
	}

	if (negative)
		out = -value;
	else
		out = value;

	return in;
}

//! Convert a string to a floating point number
//! \param floatAsString: The string to convert.
inline float fast_atof(const char* floatAsString)
{
	float ret;
	fast_atof_move(floatAsString, ret);
	return ret;
}

// random noise [-1;1]
struct Noiser
{
	static float get()
	{
		static unsigned int RandomSeed = 0x69666966;
		RandomSeed = (RandomSeed * 3631 + 1);

		float value = ((float)(RandomSeed & 0x7FFF) * (1.0f / (float)(0x7FFF >> 1))) - 1.f;
		return value;
	}
};

inline static uint32_t generateHashValue(const char *fname, const int size) {
	int		i;
	uint32_t	hash;

	hash = 0;
	i = 0;
	while (fname[i] != '\0') {
		char letter = (char)tolower(fname[i]);
		if (letter == '.') break;				// don't include extension
		if (letter == '\\') letter = '/';		// damn path names
		hash += (uint32_t)(letter)*(i + 119);
		++i;
	}
	hash = (hash ^ (hash >> 10) ^ (hash >> 20));
	hash &= (size - 1);
	return hash;
}

inline static float KernelBSpline(const float x)
{
	if (x > 2.0f)
		return 0.0f;

	float xm1 = x - 1.0f;
	float xp1 = x + 1.0f;
	float xp2 = x + 2.0f;

	float a = (xp2 <= 0.0f) ? 0.0f : (xp2*xp2*xp2);
	float b = (xp1 <= 0.0f) ? 0.0f : (xp1*xp1*xp1);
	float c = (x <= 0) ? 0.0f : (x*x*x);
	float d = (xm1 <= 0.0f) ? 0.0f : (xm1*xm1*xm1);

	return (0.16666666666666666667f * (a - (4.0f * b) + (6.0f * c) - (4.0f * d)));
}

template<class T>
inline T interpolate(const float r, const T &v1, const T &v2)
{
	return static_cast<T>(v1*(1.0f - r) + v2*r);
}

template<class T>
inline  T interpolateHermite(const float r, const T &v1, const T &v2, const T &in, const T &out)
{
	// basis functions
	float h1 = 2.0f*r*r*r - 3.0f*r*r + 1.0f;
	float h2 = -2.0f*r*r*r + 3.0f*r*r;
	float h3 = r*r*r - 2.0f*r*r + r;
	float h4 = r*r*r - r*r;

	// interpolation
	return static_cast<T>(v1*h1 + v2*h2 + in*h3 + out*h4);
}

inline const char* strstr_anyof(const char* str, const char* substring1, const char* substring2)
{
	const char* t1 = strstr(str, substring1);
	const char* t2 = strstr(str, substring2);

	if (!t1)
		return t2;
	if (!t2)
		return t1;
	if (t1 < t2)
		return t1;
	if (t1 >= t2)
		return t2;
	return NULL;
}

inline void fixWin10Console()
{
#ifdef A_PLATFORM_WIN_DESKTOP
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE;
	mode &= ~ENABLE_INSERT_MODE;
	mode &= ~ENABLE_MOUSE_INPUT;
	SetConsoleMode(hStdin, mode);
#endif
}