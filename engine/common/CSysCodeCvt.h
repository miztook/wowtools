#pragma once

#include "predefine.h"
#include <string>
#include <memory>
#include <limits>

#ifdef A_PLATFORM_WIN_DESKTOP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#else
#include <pthread.h>
#endif

class CSysCodeCvt
{
public:

	// 字符编码转换函数。
	// 字符编码转换函数 Part I: 单个字符UTF32 <-> UTF8，自己实现的。
	struct UTF8_EncodedChar
	{
		UTF8_EncodedChar()
		{
			memset(bytes, 0, 8);
		}
		int GetByteCount()
		{
			return len > 6 ? 6 : len;
		}
		union
		{
			char bytes[8];
			struct
			{
				char byte0;
				char byte1;
				char byte2;
				char byte3;
				char byte4;
				char byte5;
				char byte6; // always null
				uint8_t len;
			};
		};
	};
	static int32_t ParseUnicodeFromUTF8Str(const char* szUTF8, int32_t* pnAdvancedInUtf8Str = 0, uint32_t nUtf8StrMaxLen = UINT_MAX);
	static UTF8_EncodedChar EncodeUTF8(int32_t ch);
	static int32_t ParseUnicodeFromUTF8StrEx(const char* szUTF8, int32_t iParsePos = 0, int32_t* piParsedHeadPos = 0, int32_t* pnUtf8ByteCounts = 0, uint32_t nUtf8StrMaxLen = UINT_MAX);
	// 字符编码转换函数 Part II: UTF16LE <-> UTF8，自己实现的。
	static int32_t UTF16Len(const char16_t* sz16); // returns the auint16-char count NOT including \0.
	static int32_t UTF8ToUTF16LE(char16_t* sz16, const char* sz8); // returns the auint16-char count including \0 of the converted string.
	static int32_t UTF16LEToUTF8(char* sz8, const char16_t* sz16); // returns the byte count including \0 of the converted string.
};

//AThreadLocal
#ifdef A_PLATFORM_WIN_DESKTOP

#define AThreadLocal(of_type) __declspec(thread) of_type

#else

#include <pthread.h>
template<typename T>
struct AngelicaThreadLocal
{
	AngelicaThreadLocal()
	{
		pthread_key_create(&key, AngelicaThreadLocal<T>::_ClearStorage);
	}
	AngelicaThreadLocal(const T& other) : AngelicaThreadLocal()
	{
		m_InitData = other;
	}
	AngelicaThreadLocal<T>& operator=(const T& other)
	{
		Data() = other;
		return *this;
	}
	operator const T&() const
	{
		return Data();
	}
	operator T&()
	{
		return Data();
	}

private:
	pthread_key_t key;
	T m_InitData;
	static void _ClearStorage(void* pData)
	{
		if (pData)
			delete (T*)pData;
	}
	T& Data() const
	{
		void* data = pthread_getspecific(key);
		if (!data)
		{
			data = new T(m_InitData);
			pthread_setspecific(key, data);
		}
		return *(T*)data;
	}
};
#define AThreadLocal(of_type) AngelicaThreadLocal<of_type>

#endif

// ## Classes for string convert macros.
class BaseStackStringConverter
{
protected:

	BaseStackStringConverter(void* pAllocaBuffer)
	{
		nBufferLen = nBufferLenTemp;
		szBuffer = pAllocaBuffer;
	}

public:
	operator char*() const { return (char*)szBuffer; }
	operator char16_t*() const { return (char16_t*)szBuffer; }
	operator void*() const { return szBuffer; }

	static AThreadLocal(size_t) nBufferLenTemp;
	static AThreadLocal(const void*) szConvertSrcTemp;
	static AThreadLocal(size_t) nSrcLenTemp;

	static size_t Prepare(const std::string& rStr)
	{
		szConvertSrcTemp = rStr.c_str();
		nSrcLenTemp = rStr.length();
		return nSrcLenTemp;
	}

	static size_t Prepare(const BaseStackStringConverter& rStr)
	{
		szConvertSrcTemp = (const char*)rStr;
		nSrcLenTemp = rStr.GetLength();
		return nSrcLenTemp;
	}

	static size_t Prepare(const char* rStr)
	{
		szConvertSrcTemp = rStr;
		nSrcLenTemp = strlen(rStr);
		return nSrcLenTemp;
	}

	static size_t PrepareUTF16_UTF8(const char16_t* rStr)
	{
		szConvertSrcTemp = (const char*)rStr;
		nSrcLenTemp = CSysCodeCvt::UTF16Len(rStr);
		return nSrcLenTemp * 4 + 1;
	}

	static size_t PrepareUTF8_UTF16(const char* rStr)
	{
		szConvertSrcTemp = rStr;
		nSrcLenTemp = strlen(rStr);
		return nSrcLenTemp * 2 + 2;
	}

	size_t GetLength() const { return nBufferLen; }

protected:

	size_t nBufferLen;
	void* szBuffer;
};

class UTF16ToUTF8Converter : public BaseStackStringConverter
{
public:
	UTF16ToUTF8Converter(void* pAllocaBuffer) : BaseStackStringConverter(pAllocaBuffer) { Convert(szConvertSrcTemp); }
protected:
	void Convert(const void* szSrc) { nBufferLen = CSysCodeCvt::UTF16LEToUTF8((char*)szBuffer, (const char16_t*)szSrc); }
};

class UTF8ToUTF16Converter : public BaseStackStringConverter
{
public:
	UTF8ToUTF16Converter(void* pAllocaBuffer) : BaseStackStringConverter(pAllocaBuffer) { Convert(szConvertSrcTemp); }
protected:
	void Convert(const void* szSrc) { nBufferLen = CSysCodeCvt::UTF8ToUTF16LE((char16_t*)szBuffer, (const char*)szSrc); }
};
// =# Classes for string convert macros. ^above^

// ## Helper Macro To Convert String To Different Encodings.
// The return values of these macros should NOT be released.

// Convert from UTF-16 to UTF-8. Returned pointer should NOT be released.
#define A_UTF16_TO_UTF8(x) (char*)UTF16ToUTF8Converter(AAlloca16(BaseStackStringConverter::nBufferLenTemp = BaseStackStringConverter::PrepareUTF16_UTF8((const char16_t*)(x))))
// Convert from UTF-8 to UTF-16. Returned pointer should NOT be released.
#define A_UTF8_TO_UTF16(x) (char16_t*)(void*)UTF8ToUTF16Converter(AAlloca16(BaseStackStringConverter::nBufferLenTemp = BaseStackStringConverter::PrepareUTF8_UTF16(x)))

// Convert from UTF-16 to UTF-8. Returned pointer should NOT be released.
#define ASTR_UTF16_TO_UTF8(x) UTF16ToUTF8Converter(AAlloca16(BaseStackStringConverter::nBufferLenTemp = ASysCodeCvt::UTF16LEToUTF8(0, (const char16_t*)(const void*)(BaseStackStringConverter::szConvertSrcTemp = (x)))))
// Convert from UTF-8 to UTF-16. Returned pointer should NOT be released.
#define ASTR_UTF8_TO_UTF16(x) (char16_t*)(void*)UTF8ToUTF16Converter(AAlloca16(BaseStackStringConverter::nBufferLenTemp = BaseStackStringConverter::Prepare(x) * 2 + 2))
// =# Helper Macro To Convert String To Different Encodings. ^above^