#include "CFTFont.h"
#include "Engine.h"
#include "CFTGlyphCache.h"
#include "CFontManager.h"

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

	Texts.reserve(1024);
	DrawTexts.reserve(32);
	m_TmpVerts.reserve(512);
}

CFTFont::~CFTFont()
{
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