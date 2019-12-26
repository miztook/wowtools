#pragma once

#include "base.h"
#include "rect.h"
#include "SColor.h"
#include "vector2d.h"
#include "S3DVertex.h"
#include <vector>
#include <map>

#include "FontDef.h"
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H


class CFontManager;
class CFTGlyphCache;
struct S2DBlendParam;
class ITexture;

class CFTFont
{
public:
	explicit CFTFont(CFontManager* fontManager, const char* faceName, int faceIndex, uint32_t size, int fontStyle, int outlineWidth);
	~CFTFont();

public:
	const static int FONT_TEXTURE_SIZE = 512;
	const static int INTER_GLYPH_PAD_SPACE = 0;
	const static int INTER_LINE_PADDING = 0;

public:
	uint32_t getFontSize() const { return FontSize; }
	const char* getFontFacePath() const { return FontFacePath.c_str(); }
	int getFontFaceIndex() const { return FontFaceIndex; }
	int getFontStyle() const { return FontStyle; }
	int getOutlineWidth() const { return OutlineWidth; }
	int getFontWidth() const { return m_iFontWidth; }
	int getFontHeight() const { return m_iFontHeight; }
	bool isFixedSize() const { return m_bFixedSize; }
	void setFixedSize(bool bFixed) { m_bFixedSize = bFixed; }

public:
	struct SCharInfo
	{
		SCharInfo() : TexIndex(-1), offsetX(0), offsetY(0), width(0), height(0) {}

		recti		UVRect;		//in texture
		int		TexIndex;
		int16_t		offsetX;
		int16_t		offsetY;
		int16_t		width;
		int16_t		height;
	};

public:
	void drawA(const char* utf8text, SColor color, vector2di position, int nCharCount = -1);
	void drawW(const char16_t* text, SColor color, vector2di position, int nCharCount = -1);

	dimension2d getTextExtent(const char* utf8text, int nCharCount = -1, bool vertical = false);
	dimension2d getWTextExtent(const char16_t* text, int nCharCount = -1, bool vertical = false);
	dimension2d getWCharExtent(char16_t ch, bool vertical = false);
	int GetTextWCount(const char* utf8text) const;

	FT_BitmapGlyph getCharExtent(uint32_t ch, int& offsetX, int& offsetY, int& chWidth, int& chHeight, int& chBmpW, int& chBmpH);
	const SCharInfo* getCharInfo(char16_t ch) const;
	const SCharInfo* addChar(char16_t ch);
	ITexture* getTexture(uint32_t idx);
	uint32_t getTextureCount() const { return (uint32_t)FontTextures.size(); }
	const CFontManager* getFontManager() const { return FontManager; }

public:
	bool init();

	int GetUnicodeCMapIndex(FT_Face ftFace) const;
	My_FaceID GetMyFaceID() const { return MyFaceID; }
	FTC_Scaler GetFTCScaler() const { return FTCScaler; }
	FT_Face GetFTFace() const;
	FT_Size GetFTSize() const;
	FT_Pos GetBoldWidth() const { return BoldWidth; }

private:
	struct SDrawText
	{
		uint32_t		offset;
		uint32_t		length;
		SColor color;
		vector2di position;
		recti		rcClip;
		bool		bClip;
		bool		bVertical;
		//TODO underline
	};

private:
	bool addFontTexture();
	FT_BitmapGlyph renderChar(uint32_t ch, bool bRenderAsOutline);

private:
	const std::string	FontFacePath;
	const int FontFaceIndex;
	const uint32_t FontSize;
	const int FontStyle;
	const int OutlineWidth;

	//字体最大高度宽度
	int		m_iFontWidth;
	int		m_iFontHeight;

	bool		m_bFixedSize;

	//
private:
	using T_CharacterMap = std::map<char16_t, SCharInfo>;

	T_CharacterMap		CharacterMap;

	CFontManager*		FontManager;
	CFTGlyphCache*		MyFTGlyphCache;

	My_FaceID	MyFaceID;
	FTC_Scaler	FTCScaler;
	FT_Pos		BoldWidth;
	int		UnicodeCMapIndex;

	int32_t		Ascender;				//overhang
	int32_t		Descender;			//underhang

	std::vector<ITexture*>			FontTextures;

	int32_t	NextX;
	int32_t  NextY;
	int32_t  CurrentX;
	int32_t  CurrentY;
	int32_t  MaxY;

	//
	std::vector<char16_t>		Texts;
	std::vector<SDrawText>	DrawTexts;
	std::vector<SVertex_PCT>	m_TmpVerts;
};