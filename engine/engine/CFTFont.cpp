#include "CFTFont.h"
#include "Engine.h"
#include "CFTGlyphCache.h"
#include "CFontManager.h"
#include "CSysCodeCvt.h"
#include "ITexture.h"
#include "ITextureWriter.h"
#include "CFileSystem.h"

#define FONTSIZE_ROUND(x)  (((x) + 32) & ~63)
#define FONTSIZE_FLOOR(x) ((x) & ~63)
#define FONTSIZE_CEILING(x)  (((x) + 63) & ~63)

CFTFont::CFTFont(CFontManager* fontManager, const char* faceName, int faceIndex, uint32_t size, int fontStyle, int outlineWidth)
	: FontManager(fontManager), FontFacePath(faceName), FontFaceIndex(faceIndex), FontSize(size), FontStyle(fontStyle), OutlineWidth(outlineWidth)
{
	m_iFontWidth = m_iFontHeight = 0;
	m_bFixedSize = false;

	Ascender =
		Descender = 0;

	NextX = NextY = CurrentX = CurrentY = MaxY = 0;

	MyFTGlyphCache = nullptr;

	MyFaceID = nullptr;
	FTCScaler = nullptr;
	UnicodeCMapIndex = -1;
	BoldWidth = 0;
}

CFTFont::~CFTFont()
{
	for (ITexture* tex : FontTextures)
	{
		if (tex)
		{
			g_Engine->getDriver()->removeTextureWriter(tex);
			delete tex;
		}
	}

	delete MyFTGlyphCache;
	delete FTCScaler;
	if (MyFaceID)
		FontManager->RemoveFaceID(MyFaceID);
}

void CFTFont::drawA(const char* utf8text, SColor color, vector2di position, int nCharCount /*= -1*/)
{
	int32_t x = position.x;
	int32_t y = position.y;

	const S2DBlendParam& blendParam = S2DBlendParam::UIFontBlendParam();
	float fInv = 1.0f / FONT_TEXTURE_SIZE;

	const char* p = utf8text;
	int nLen = nCharCount > 0 ? nCharCount : (int)strlen(utf8text);

	SVertex_PCT verts[4];			//left top, right top, left bottom, right bottom
	while (*p && nLen >= 0)
	{
		char16_t ch;
		int nadv;
		ch = (char16_t)CSysCodeCvt::ParseUnicodeFromUTF8Str(p, &nadv, nLen);
		if (nadv == 0)		//parse end
			break;
		p += nadv;
		nLen -= nadv;
		if (ch == 0)		//string end
			break;

		const SCharInfo* charInfo = addChar(ch);

		if (!charInfo)
			continue;

		if (ch == (char16_t)'\r' || ch == (char16_t)'\n') // Unix breaks
		{
			y += m_iFontHeight;
			x = position.x;
			continue;
		}

		int idx = charInfo->TexIndex;
		ASSERT(idx >= 0 && idx < (int32_t)FontTextures.size());

		ITexture* texture = FontTextures[idx];
		float posX0 = (float)(x + charInfo->offsetX);
		float posX1 = (float)(posX0 + charInfo->UVRect.getWidth());
		float posY0 = (float)(y + charInfo->offsetY);
		float posY1 = (float)(posY0 + charInfo->UVRect.getHeight());

		float texX0 = charInfo->UVRect.left * fInv;
		float texX1 = charInfo->UVRect.right * fInv;
		float texY0 = charInfo->UVRect.top * fInv;
		float texY1 = charInfo->UVRect.bottom * fInv;

		float fLeft = posX0;
		float fTop = posY0;
		float fRight = posX1;
		float fBottom = posY1;
		float tu1 = 0.0f;
		float tu2 = 0.0f;
		float tv1 = 0.0f;
		float tv2 = 0.0f;

		float du = texX1 - texX0;
		float dv = texY1 - texY0;

		verts[0].Pos.set(fLeft, fTop, 0);
		verts[0].Color = color;
		verts[0].TCoords.set(
			texX0 + tu1 * du,
			texY0 + tv1 * dv);

		verts[1].Pos.set(fRight, fTop, 0);
		verts[1].Color = color;
		verts[1].TCoords.set(
			texX1 - tu2 * du,
			texY0 + tv1 * dv);

		verts[2].Pos.set(fLeft, fBottom, 0);
		verts[2].Color = color;
		verts[2].TCoords.set(
			texX0 + tu1 * du,
			texY1 - tv2 * dv);

		verts[3].Pos.set(fRight, fBottom, 0);
		verts[3].Color = color;
		verts[3].TCoords.set(
			texX1 - tu2 * du,
			texY1 - tv2 * dv);

		g_Engine->getDriver()->add2DQuads(texture,
			&verts[0],
			1,
			blendParam);

		x += charInfo->width;
	}

	g_Engine->getDriver()->flushAll2DQuads();
}

void CFTFont::drawW(const char16_t* text, SColor color, vector2di position, int nCharCount /*= -1*/)
{
	int32_t x = position.x;
	int32_t y = position.y;

	const S2DBlendParam& blendParam = S2DBlendParam::UIFontBlendParam();
	float fInv = 1.0f / FONT_TEXTURE_SIZE;

	uint32_t len = nCharCount > 0 ? (uint32_t)nCharCount : (uint32_t)CSysCodeCvt::UTF16Len(text);

	SVertex_PCT verts[4];			//left top, right top, left bottom, right bottom
	for (uint32_t i = 0; i < len; ++i)
	{
		char16_t c = text[i];
		if (c == 0)		//string end
			continue;

		const SCharInfo* charInfo = addChar(c);

		if (!charInfo)
			continue;

		if (c == (char16_t)'\r' || c == (char16_t)'\n') // Unix breaks
		{
			y += m_iFontHeight;
			x = position.x;
			continue;
		}

		int idx = charInfo->TexIndex;
		ASSERT(idx >= 0 && idx < (int32_t)FontTextures.size());

		ITexture* texture = FontTextures[idx];

		float posX0 = (float)(x + charInfo->offsetX);
		float posX1 = (float)(posX0 + charInfo->UVRect.getWidth());
		float posY0 = (float)(y + charInfo->offsetY);
		float posY1 = (float)(posY0 + charInfo->UVRect.getHeight());

		float texX0 = charInfo->UVRect.left * fInv;
		float texX1 = charInfo->UVRect.right * fInv;
		float texY0 = charInfo->UVRect.top * fInv;
		float texY1 = charInfo->UVRect.bottom * fInv;

		float fLeft = posX0;
		float fTop = posY0;
		float fRight = posX1;
		float fBottom = posY1;
		float tu1 = 0.0f;
		float tu2 = 0.0f;
		float tv1 = 0.0f;
		float tv2 = 0.0f;

		float du = texX1 - texX0;
		float dv = texY1 - texY0;

		verts[0].Pos.set(fLeft, fTop, 0);
		verts[0].Color = color;
		verts[0].TCoords.set(
			texX0 + tu1 * du,
			texY0 + tv1 * dv);

		verts[1].Pos.set(fRight, fTop, 0);
		verts[1].Color = color;
		verts[1].TCoords.set(
			texX1 - tu2 * du,
			texY0 + tv1 * dv);

		verts[2].Pos.set(fLeft, fBottom, 0);
		verts[2].Color = color;
		verts[2].TCoords.set(
			texX0 + tu1 * du,
			texY1 - tv2 * dv);

		verts[3].Pos.set(fRight, fBottom, 0);
		verts[3].Color = color;
		verts[3].TCoords.set(
			texX1 - tu2 * du,
			texY1 - tv2 * dv);

		g_Engine->getDriver()->add2DQuads(texture,
			&verts[0],
			1,
			blendParam);

		x += charInfo->width;
	}

	g_Engine->getDriver()->flushAll2DQuads();
}

dimension2d CFTFont::getTextExtent(const char* utf8text, int nCharCount /*= -1*/, bool vertical /*= false*/)
{
	int32_t x;
	int32_t y;

	if (vertical)
	{
		x = m_iFontWidth;
		y = 0;
	}
	else
	{
		x = 0;
		y = m_iFontHeight;
	}

	const char* sz8 = utf8text;
	int nLen = nCharCount > 0 ? nCharCount : (int)strlen(utf8text);
	while (*sz8 && nLen >= 0)
	{
		int32_t nAdv;
		int32_t value = CSysCodeCvt::ParseUnicodeFromUTF8Str(sz8, &nAdv, nLen);
		if (nAdv == 0)
			break;
		sz8 += nAdv;
		nLen -= nAdv;
		if (value < 0 || value >= 0x10000)
			continue;

		char16_t ch = (char16_t)value;

		const SCharInfo* charInfo = addChar(ch);
		ASSERT(charInfo);

		if (ch == (char16_t)'\r' || ch == (char16_t)'\n') // Unix breaks
		{
			// 			if (vertical)
			// 			{
			// 				x += charInfo->width;
			// 				y = 0;
			// 			}
			y += m_iFontHeight;
			x = 0;
		}
		else
		{
			if (vertical)
				y += m_iFontHeight;
			else
				x += charInfo->width;
		}
	}

	return dimension2d(x, y);
}

dimension2d CFTFont::getWTextExtent(const char16_t* text, int nCharCount /*= -1*/, bool vertical /*= false*/)
{
	int32_t x;
	int32_t y;

	if (vertical)
	{
		x = m_iFontWidth;
		y = 0;
	}
	else
	{
		x = 0;
		y = m_iFontHeight;
	}

	uint32_t len = nCharCount > 0 ? (uint32_t)nCharCount : (uint32_t)CSysCodeCvt::UTF16Len(text);
	for (uint32_t i = 0; i < len; ++i)
	{
		char16_t ch = text[i];
		const SCharInfo* charInfo = addChar(ch);
		ASSERT(charInfo);

		if (ch == (char16_t)'\r' || ch == (char16_t)'\n') // Unix breaks
		{
			if (vertical)
			{
				x += charInfo->width;
				y = 0;
			}
			else
			{
				y += m_iFontHeight;
				x = 0;
			}
		}
		else
		{
			if (vertical)
				y += m_iFontHeight;
			else
				x += charInfo->width;
		}
	}

	return dimension2d(x, y);
}

dimension2d CFTFont::getWCharExtent(char16_t ch, bool vertical /*= false*/)
{
	int32_t x;
	int32_t y;

	if (vertical)
	{
		x = m_iFontWidth;
		y = 0;
	}
	else
	{
		x = 0;
		y = m_iFontHeight;
	}

	const SCharInfo* charInfo = addChar(ch);
	ASSERT(charInfo);

	if (ch == (char16_t)'\r' || ch == (char16_t)'\n') // Mac or Windows breaks
	{
		// 		if (vertical)
		// 		{
		// 			x += charInfo->width;
		// 			y = 0;
		// 		}
		// 		else
		y += m_iFontHeight;
		x = 0;
	}
	else
	{
		if (vertical)
			y += m_iFontHeight;
		else
			x += charInfo->width;
	}

	return dimension2d(x, y);
}

int CFTFont::GetTextWCount(const char * utf8text) const
{
	int uCharCount = 0;
	const char* p = utf8text;
	while (*p)
	{
		char16_t ch;
		int nadv;
		ch = (char16_t)CSysCodeCvt::ParseUnicodeFromUTF8Str(p, &nadv);
		if (nadv == 0)		//parse end
			break;
		p += nadv;
		if (ch == 0)		//string end
			break;
		++uCharCount;
	}

	return uCharCount;
}

FT_BitmapGlyph CFTFont::getCharExtent(uint32_t ch, int& offsetX, int& offsetY, int& chWidth, int& chHeight, int& chBmpW, int& chBmpH)
{
	FT_BitmapGlyph ftGlyph;

	ftGlyph = renderChar(ch, false);

	if (!ftGlyph)
		return nullptr;

	FT_Size ftSize = GetFTSize();

	offsetX = ftGlyph->left;
	offsetY = (((ftSize->metrics.ascender << 1) + BoldWidth) >> 7) - ftGlyph->top;

	chWidth = FixedToInt(ftGlyph->root.advance.x) + (BoldWidth >> 6);
	chHeight = m_iFontHeight;

	chBmpW = ftGlyph->bitmap.width;
	chBmpH = ftGlyph->bitmap.rows;

	return ftGlyph;
}

const CFTFont::SCharInfo* CFTFont::getCharInfo(char16_t ch) const
{
	auto itr = CharacterMap.find(ch);
	if (itr != CharacterMap.end())
		return &itr->second;
	return nullptr;
}

const CFTFont::SCharInfo* CFTFont::addChar(char16_t ch)
{
	T_CharacterMap::const_iterator itr = CharacterMap.find(ch);
	if (itr != CharacterMap.end())
	{
		ASSERT((itr->second).TexIndex >= 0 &&
			(itr->second).TexIndex <= (int32_t)FontTextures.size());
		return &itr->second;
	}

	FT_BitmapGlyph bitmapGlyph;
	int offsetX, offsetY, chW, chH, bmpW, bmpH;

	bitmapGlyph = getCharExtent(ch, offsetX, offsetY, chW, chH, bmpW, bmpH);
	if (!bitmapGlyph)
	{
		ASSERT(false);
		return nullptr;
	}

	ASSERT(bmpW <= m_iFontWidth);
	ASSERT(bmpH <= m_iFontHeight);

	NextX = CurrentX + (m_iFontWidth + INTER_GLYPH_PAD_SPACE);

	if (NextX > FONT_TEXTURE_SIZE)				//换行
	{
		CurrentX = 0;
		NextX = CurrentX + (m_iFontWidth + INTER_GLYPH_PAD_SPACE);
		CurrentY = NextY;
	}

	MaxY = CurrentY + (m_iFontHeight + INTER_LINE_PADDING);						//换页, 增加纹理
	if (MaxY > FONT_TEXTURE_SIZE)
	{
		addFontTexture();

		CurrentX = CurrentY = NextY = 0;
		MaxY = 0;
	}

	//write texture
	ITexture* tex = FontTextures.back();

	int32_t charposx = CurrentX;
	int32_t charposy = CurrentY;

	recti rc(charposx, charposy, charposx + m_iFontWidth, charposy + m_iFontHeight);

	if (!rc.isEmpty())			//如空格字符的rect为空，不需要绘制
	{
		//CLock lock(&g_Globals.textureCS);

		ITextureWriter* texWriter = g_Engine->getDriver()->createTextureWriter(tex);

		uint32_t pitch;
		uint8_t* data = (uint8_t*)texWriter->lock(0, 0, pitch);
		ASSERT(data);

		//写纹理 A8L8
		if (tex->getColorFormat() == ECF_A8L8)
		{
			uint8_t* p = data + (charposy * pitch) + getBytesPerPixelFromFormat(ECF_A8L8) * charposx;

			for (int h = 0; h < m_iFontHeight; ++h)
			{
				uint8_t* dst = reinterpret_cast<uint8_t*>(p);
				for (int w = 0; w < m_iFontWidth; ++w)
				{
					bool bTestEdge = (h == 0 || h == m_iFontHeight - 1 || w == 0 || w == m_iFontWidth - 1);
					bool bEdge = (w >= bmpW || h >= bmpH);
					if (bEdge)
					{
						uint8_t alpha = 0;

						*dst++ = 0;		//color
						*dst++ = 0;		//alpha
					}
					else
					{
						const uint8_t* src = bitmapGlyph->bitmap.buffer + (h * bitmapGlyph->bitmap.pitch);
						uint8_t alpha = src[w];

						*dst++ = alpha; 	//color
						*dst++ = alpha;		//alpha
					}
				}
				p += pitch;
			}
		}
		else if (tex->getColorFormat() == ECF_A8R8G8B8)			//dx11 format
		{
			uint8_t* p = data + (charposy * pitch) + getBytesPerPixelFromFormat(ECF_A8R8G8B8) * charposx;

			for (int h = 0; h < m_iFontHeight; ++h)
			{
				uint8_t* dst = reinterpret_cast<uint8_t*>(p);
				for (int w = 0; w < m_iFontWidth; ++w)
				{
					bool bEdge = (w >= bmpW || h >= bmpH);
					if (bEdge)
					{
						uint8_t alpha = 0;

						*dst++ = alpha; 	//b
						*dst++ = alpha; 	//g
						*dst++ = alpha; 	//r

						*dst++ = alpha;		//alpha
					}
					else
					{
						const uint8_t* src = bitmapGlyph->bitmap.buffer + (h * bitmapGlyph->bitmap.pitch);
						uint8_t alpha = src[w];

						*dst++ = alpha; 	//b
						*dst++ = alpha; 	//g
						*dst++ = alpha; 	//r

						*dst++ = alpha;		//alpha
					}
				}
				p += pitch;
			}
		}
		else
		{
			ASSERT(false);
		}

		texWriter->unlock(0, 0);

		texWriter->copyToTexture(tex, &rc);
	}

	//添加信息
	SCharInfo charInfo;

	rc.set(charposx, charposy, charposx + bmpW, charposy + bmpH);
	charInfo.TexIndex = (int32_t)FontTextures.size() - 1;
	charInfo.offsetX = offsetX;
	charInfo.offsetY = offsetY;
	charInfo.width = chW;
	charInfo.height = chH;
	charInfo.UVRect = rc;

	CharacterMap[ch] = charInfo;

	CurrentX = NextX;

	if (MaxY > NextY)
		NextY = MaxY;

	return &CharacterMap[ch];
}

ITexture* CFTFont::getTexture(uint32_t idx)
{
	if (idx >= (uint32_t)FontTextures.size())
		return nullptr;

	return FontTextures[idx];
}

bool CFTFont::init()
{
	MyFaceID = FontManager->LookupFaceID(FontFacePath.c_str(), FontFaceIndex, FontStyle);

	FT_Face ftFace;

	if (FTC_Manager_LookupFace(FontManager->GetCacheManager(), (FTC_FaceID)MyFaceID, &ftFace))
	{
		ASSERT(false);
		return false;
	}

	if (-1 == (UnicodeCMapIndex = GetUnicodeCMapIndex(ftFace)))
	{
		ASSERT(false);
		return false;
	}

	//
	FTCScaler = new FTC_ScalerRec;
	FTCScaler->face_id = (FTC_FaceID)MyFaceID;
	FTCScaler->width = (FontSize << 6);
	FTCScaler->height = (FontSize << 6);
	FTCScaler->pixel = 0;
	FTCScaler->x_res = DEFAULT_DPI;
	FTCScaler->y_res = DEFAULT_DPI;

	FT_Size ftSize;
	if (FTC_Manager_LookupSize(FontManager->GetCacheManager(), FTCScaler, &ftSize))
	{
		ASSERT(false);
		return false;
	}

	MyFTGlyphCache = new CFTGlyphCache(this);

	if (FontStyle & STYLE_BOLD)
	{
		// fix bold font glyph size. - Silas, 2013/03/08
		BoldWidth = FT_MulFix(ftFace->units_per_EM,
			ftSize->metrics.y_scale) / 24;
	}
	else
	{
		BoldWidth = 0;
	}

	Ascender = (int32_t)(((ftSize->metrics.ascender << 1) + BoldWidth) >> 7) + getOutlineWidth();
	Descender = (int32_t)(((ftSize->metrics.descender << 1) + BoldWidth) >> 7) + getOutlineWidth();  // 基线到字符轮廓最低点的距离

																									 //(((ftFace->bbox.xMax - ftFace->bbox.xMin) * ftSize->metrics.x_scale) >> 22)
	m_iFontWidth = FT_MulFix((FONTSIZE_CEILING(ftFace->bbox.xMax) - FONTSIZE_FLOOR(ftFace->bbox.xMin)), ftSize->metrics.x_scale) / 64 + getOutlineWidth() * 2 + (BoldWidth >> 6);
	m_iFontHeight = FT_MulFix((FONTSIZE_CEILING(ftFace->bbox.yMax) - FONTSIZE_FLOOR(ftFace->bbox.yMin)), ftSize->metrics.y_scale) / 64 + getOutlineWidth() * 2 + (BoldWidth >> 6);

	int offsetX, offsetY, chW, chH, bmpW, bmpH;
	if (getCharExtent('W', offsetX, offsetY, chW, chH, bmpW, bmpH))
	{
		if (bmpW > m_iFontWidth)
			m_iFontWidth = bmpW;
	}
	if (getCharExtent('j', offsetX, offsetY, chW, chH, bmpW, bmpH))
	{
		if (bmpH > m_iFontHeight)
			m_iFontHeight = bmpH;
	}

	addFontTexture();

	return true;
}

int CFTFont::GetUnicodeCMapIndex(FT_Face ftFace) const
{
	for (int i = 0; i < ftFace->num_charmaps; i++)
	{
		if (FT_ENCODING_UNICODE == ftFace->charmaps[i]->encoding)
			return i;
	}
	return -1;
}

FT_Face CFTFont::GetFTFace() const
{
	FT_Face ftFace = nullptr;
	FTC_Manager_LookupFace(FontManager->GetCacheManager(), (FTC_FaceID)MyFaceID, &ftFace);
	return ftFace;
}

FT_Size CFTFont::GetFTSize() const
{
	FT_Size ftSize = nullptr;
	FTC_Manager_LookupSize(FontManager->GetCacheManager(), FTCScaler, &ftSize);
	return ftSize;
}

bool CFTFont::addFontTexture()
{
	ECOLOR_FORMAT format;

	if (g_Engine->getDriver()->isSupportA8L8())
	{
		format = ECF_A8L8;
	}
	else
	{
		format = ECF_A8R8G8B8;
	}

	ITexture* fontTex = g_Engine->getDriver()->createEmptyTexture(dimension2d(FONT_TEXTURE_SIZE, FONT_TEXTURE_SIZE), format);	   //no mipmap
	FontTextures.push_back(fontTex);

	return true;
}

FT_BitmapGlyph CFTFont::renderChar(uint32_t ch, bool bRenderAsOutline)
{
	FTC_CMapCache mapCache = FontManager->GetCMapCache();
	FTC_FaceID ftFaceID = (FTC_FaceID)GetMyFaceID();

	//在cmap中找
	uint32_t index = FTC_CMapCache_Lookup(mapCache, ftFaceID, UnicodeCMapIndex, (uint32_t)ch);
	if (0 == index)
	{
		index = FTC_CMapCache_Lookup(mapCache, ftFaceID, UnicodeCMapIndex, L'□');
		if (0 == index)
			return nullptr;
	}

	return MyFTGlyphCache->LookupGlyph(index, bRenderAsOutline);
}

