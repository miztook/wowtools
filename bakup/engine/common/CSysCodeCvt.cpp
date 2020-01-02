#include "CSysCodeCvt.h"

AThreadLocal(size_t) BaseStackStringConverter::nBufferLenTemp;
AThreadLocal(const void*) BaseStackStringConverter::szConvertSrcTemp;
AThreadLocal(size_t) BaseStackStringConverter::nSrcLenTemp;

int32_t CSysCodeCvt::ParseUnicodeFromUTF8Str(const char* szUTF8, int32_t* pnAdvancedInUtf8Str, uint32_t nUtf8StrMaxLen)
{
	uint32_t nAdv = 0;
	int32_t nResult = 0;
	if (pnAdvancedInUtf8Str != 0)
		*pnAdvancedInUtf8Str = 0;

	// Check
	if (szUTF8 == 0)
		return 0;
	if (*szUTF8 == 0)
		return 0;
	if (nUtf8StrMaxLen == 0)
		return 0;

	// Byte count to combine.
	char ch = *szUTF8;
	if ((ch & 0x80) == 0)
		nAdv = 1;
	else
	{
		for (int i = 0; i < 8; ++i)
		{
			if (ch & (0x80 >> i))
				++nAdv;
			else
				break;
		}
		if (nAdv == 1)
		{ // start with 10XX XXXX ? error
			if (pnAdvancedInUtf8Str != 0)
				*pnAdvancedInUtf8Str = 1;
			return -1;
		}
	}
	if (nAdv > nUtf8StrMaxLen)
	{
		// the str is not complete.
		return 0;
	}
	if (nAdv > 6)
	{ // start with 1111 111X ? error
		if (pnAdvancedInUtf8Str != 0)
			*pnAdvancedInUtf8Str = 1;
		return -1;
	}
	if (pnAdvancedInUtf8Str != 0)
		*pnAdvancedInUtf8Str = nAdv;

	// Parse
	if (nAdv == 1)
	{
		nResult = ch & 0x7F;
	}
	else
	{
		nResult = ch & (0xFF >> (nAdv + 1));
		for (uint32_t i = 1; i < nAdv; ++i)
		{
			ch = szUTF8[i];
			if ((ch & 0xC0) != 0x80)
			{ // not 10XX XXXX
				return -1;
			}
			nResult = nResult << 6;
			nResult |= (ch & 0x3F);
		}
	}
	return nResult;
}

int32_t CSysCodeCvt::ParseUnicodeFromUTF8StrEx(const char* szUTF8, int32_t iParsePos, int32_t* piParsedHeadPos, int32_t* pnUtf8ByteCounts, uint32_t nUtf8StrMaxLen)
{
	int32_t* pnAdvancedInUtf8Str = pnUtf8ByteCounts;
	uint32_t nAdv = 0;
	int32_t nResult = 0;
	if (pnAdvancedInUtf8Str != 0)
		*pnAdvancedInUtf8Str = 0;
	if (piParsedHeadPos != 0)
		*piParsedHeadPos = -1;

	// Check
	if (szUTF8 == 0)
		return 0;
	if (iParsePos < 0)
		return 0;
	if (nUtf8StrMaxLen <= (uint32_t)iParsePos)
		return 0;

	int32_t nLen = (int32_t)strnlen(szUTF8, nUtf8StrMaxLen);
	if (nLen <= iParsePos)
		return 0;

	const char* szOriginal = szUTF8;
	szUTF8 += iParsePos;
	if (piParsedHeadPos != 0)
		*piParsedHeadPos = iParsePos;
	// try to find the head.
	do
	{
		char16_t ch = *(char16_t*)szUTF8;
		if ((ch & 0x80) == 0)
			break; // 0XXX XXXX the acsii.
		else if ((ch & 0xE0) == 0xC0)
			break; // 110X XXXX
		else if ((ch & 0xF0) == 0xE0)
			break; // 1110 XXXX
		else if ((ch & 0xF8) == 0xF0)
			break; // 1111 0XXX
		else if ((ch & 0xFC) == 0xF8)
			break; // 1111 10XX
		else if ((ch & 0xFE) == 0xFC)
			break; // 1111 110X
		else if (ch == 0xFE || ch == 0xFF)
		{ // 1111 111X
			if (pnAdvancedInUtf8Str != 0)
				*pnAdvancedInUtf8Str = 1;
			return -1;
		}
		else if ((ch & 0xC0) == 0x80)
		{ // 10XX XXXX
		  // It is in the center of a serial of UTF-8 bytes, we need to move the pos ahead to find the start of the encoded char.
			int nFindStartIndex = iParsePos - 5;
			if (nFindStartIndex < 0) nFindStartIndex = 0;
			int iFindIndex = iParsePos - 1;
			int nAdvInner = 0;
			for (; iFindIndex >= nFindStartIndex; --iFindIndex)
			{
				ch = szOriginal[iFindIndex];
				if ((ch & 0x80) == 0)
				{ // 0XXX XXXX the acsii.
					nAdvInner = 1;
					break;
				}
				else if ((ch & 0xE0) == 0xC0)
				{ // 110X XXXX
					nAdvInner = 2;
					break;
				}
				else if ((ch & 0xF0) == 0xE0)
				{ // 1110 XXXX
					nAdvInner = 3;
					break;
				}
				else if ((ch & 0xF8) == 0xF0)
				{ // 1111 0XXX
					nAdvInner = 4;
					break;
				}
				else if ((ch & 0xFC) == 0xF8)
				{ // 1111 10XX
					nAdvInner = 5;
					break;
				}
				else if ((ch & 0xFE) == 0xFC)
				{ // 1111 110X
					nAdvInner = 6;
					break;
				}
				else if ((ch & 0xC0) == 0x80)
				{ // 10XX XXXX
					continue;
				}
				else
				{ // Wrong serial.
					if (pnAdvancedInUtf8Str != 0)
						*pnAdvancedInUtf8Str = 1;
					return -1;
				}
			}
			if (iFindIndex < nFindStartIndex)
			{ // Serial start pos not found.
				if (pnAdvancedInUtf8Str != 0)
					*pnAdvancedInUtf8Str = 1;
				return -1;
			}
			else
			{
				if (iFindIndex + nAdvInner <= iParsePos)
				{ // To many 10XX XXXX serial from the head.
					if (pnAdvancedInUtf8Str != 0)
						*pnAdvancedInUtf8Str = 1;
					return -1;
				}
				else
				{
					if (piParsedHeadPos != 0)
						*piParsedHeadPos = iFindIndex;
					szUTF8 = szOriginal + iFindIndex;
				}
			}
		}
		else
		{ // unknown char
			if (pnAdvancedInUtf8Str != 0)
				*pnAdvancedInUtf8Str = 1;
			return -1;
		}
	} while (false);

	// Byte count to combine.
	char ch = *szUTF8;
	if ((ch & 0x80) == 0)
		nAdv = 1;
	else
	{
		for (int i = 0; i < 8; ++i)
		{
			if (ch & (0x80 >> i))
				++nAdv;
			else
				break;
		}
		if (nAdv == 1)
		{ // start with 10XX XXXX ? error
			if (pnAdvancedInUtf8Str != 0)
				*pnAdvancedInUtf8Str = 1;
			return -1;
		}
	}
	if (nAdv > nUtf8StrMaxLen)
	{
		// the str is not complete.
		return 0;
	}
	if (nAdv > 6)
	{ // start with 1111 111X ? error
		if (pnAdvancedInUtf8Str != 0)
			*pnAdvancedInUtf8Str = 1;
		return -1;
	}
	if (pnAdvancedInUtf8Str != 0)
		*pnAdvancedInUtf8Str = nAdv;

	// Parse
	if (nAdv == 1)
	{
		nResult = ch & 0x7F;
	}
	else
	{
		nResult = ch & (0xFF >> (nAdv + 1));
		for (uint32_t i = 1; i < nAdv; ++i)
		{
			ch = szUTF8[i];
			if ((ch & 0xC0) != 0x80)
			{ // not 10XX XXXX
				return -1;
			}
			nResult = nResult << 6;
			nResult |= (ch & 0x3F);
		}
	}
	return nResult;
}

CSysCodeCvt::UTF8_EncodedChar CSysCodeCvt::EncodeUTF8(int32_t ch)
{
	UTF8_EncodedChar ch8;
	if (ch < 0x80)
	{ // 1-byte
		ch8.len = 1;
		ch8.byte0 = (char)ch;
	}
	else if (ch < 0x800)
	{ // 2-byte
		ch8.len = 2;
		ch8.byte0 = 0xC0 | ((ch & 0x7C0) >> 6);
		ch8.byte1 = 0x80 | (ch & 0x3F);
	}
	else if (ch < 0x10000)
	{ // 3-byte
		ch8.len = 3;
		ch8.byte0 = 0xE0 | ((ch & 0xF000) >> 12);
		ch8.byte1 = 0x80 | ((ch & 0x0FC0) >> 6);
		ch8.byte2 = 0x80 | (ch & 0x3F);
	}
	else if (ch < 0x200000)
	{ // 4-byte
		ch8.len = 4;
		ch8.byte0 = 0xF0 | ((ch & 0x1C0000) >> 18);
		ch8.byte1 = 0x80 | ((ch & 0x03F000) >> 12);
		ch8.byte2 = 0x80 | ((ch & 0x0FC0) >> 6);
		ch8.byte3 = 0x80 | (ch & 0x3F);
	}
	else if (ch < 0x4000000)
	{ // 5-byte
		ch8.len = 5;
		ch8.byte0 = 0xF8 | ((ch & 0x3000000) >> 24);
		ch8.byte1 = 0x80 | ((ch & 0x0FC0000) >> 18);
		ch8.byte2 = 0x80 | ((ch & 0x03F000) >> 12);
		ch8.byte3 = 0x80 | ((ch & 0x0FC0) >> 6);
		ch8.byte4 = 0x80 | (ch & 0x3F);
	}
	else if (ch > 0)
	{ // 6-byte
		ch8.len = 6;
		ch8.byte0 = 0xFC | ((ch & 0x40000000) >> 30);
		ch8.byte1 = 0x80 | ((ch & 0x3F000000) >> 24);
		ch8.byte2 = 0x80 | ((ch & 0x0FC0000) >> 18);
		ch8.byte3 = 0x80 | ((ch & 0x03F000) >> 12);
		ch8.byte4 = 0x80 | ((ch & 0x0FC0) >> 6);
		ch8.byte5 = 0x80 | (ch & 0x3F);
	}
	else
	{ // error.
	}
	return ch8;
}

int32_t CSysCodeCvt::UTF8ToUTF16LE(char16_t* sz16, const char* sz8)
{
	if (sz8 == 0)
		return 0;
	int count = 0;
	while (*sz8 != 0)
	{
		int32_t nAdv;
		int32_t value = ParseUnicodeFromUTF8Str(sz8, &nAdv);
		if (nAdv == 0)
			break;
		sz8 += nAdv;
		if (value < 0) continue;
		++count;
		if (value >= 0x10000)
			++count;
		if (sz16 != 0)
		{
			if (value < 0x10000)
				*(sz16++) = value;
			else
			{
				uint32_t ch322 = value - 0x10000;
				*(sz16++) = ((ch322 & 0xFFC00) >> 10) | 0xD800;
				*(sz16++) = (ch322 & 0x3FF) | 0xDC00;
			}
		}
	}
	if (sz16 != 0)
		*sz16 = 0;
	return count + 1;
}

int32_t CSysCodeCvt::UTF16LEToUTF8(char* sz8, const char16_t* sz16)
{
	if (sz16 == 0)
		return 0;
	int count = 0;
	// 	if(*sz16 == (auint16)0xFEFF)
	// 	{
	// 		++sz16;
	// 	}
	while (*sz16 != 0)
	{
		int32_t nAdv;
		int32_t ch = *(sz16++);
		int32_t ch32 = ch;
		if (ch >= 0xD800 && ch <= 0xDBFF)
		{
			uint16_t ch2 = *(sz16++);
			if (ch2 >= 0xDC00 && ch2 <= 0xDFFF)
			{
				ch32 = ch & 0xD800;
				ch32 <<= 10;
				ch32 |= ch2 & 0xDC00;
			}
		}
		UTF8_EncodedChar value = EncodeUTF8(ch32);
		nAdv = value.GetByteCount();
		count += nAdv;
		if (sz8 != 0)
		{
			for (int i = 0; i < nAdv; ++i)
			{
				*(sz8++) = value.bytes[i];
			}
		}
	}
	if (sz8 != 0)
		*sz8 = 0;
	return count + 1;
}

int32_t CSysCodeCvt::UTF16Len(const char16_t* sz16)
{
	const char16_t* p = sz16;
	if (p == nullptr) return 0;
	while (*p != 0)
		++p;
	return (int32_t)(p - sz16);
}