#pragma once

#include "base.h"
#include "FontDef.h"
#include "vector2d.h"

#include <map>

#include "fontDef.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H

//在手游中用一种字体，减少复杂度
#define A_FACE_NAME		"Fonts/ARKai_C.ttf"
#define A_FACE_INDEX		0

class CFTFont;

class CFontManager
{
public:
	explicit CFontManager(int nMaxFontFamilyCountInCache, int nMaxFontStyleInCache, int nMaxCacheSizeBytes, uint32_t defaultFontSize);
	virtual ~CFontManager();

public:
	struct SFontKey
	{
		SFontKey(uint32_t _size, int _style, int _outlineWidth)
			: size(_size), style(_style), outlineWidth(_outlineWidth) {}

		uint32_t size;
		int style;
		int outlineWidth;

		bool operator<(const SFontKey& other) const
		{
			if (size != other.size)
				return size < other.size;
			else if (style != other.style)
				return style < other.style;
			else if (outlineWidth < other.outlineWidth)
				return outlineWidth < other.outlineWidth;
			return false;
		}

		bool operator==(const SFontKey& other) const
		{
			return size == other.size &&
				style == other.style &&
				outlineWidth == other.outlineWidth;
		}
	};

public:
	CFTFont* getDefaultFont()
	{
		return getFont(SFontKey(DefaultFontSize, DEFAULT_FONT_STYLE, DEFAULT_FONT_OUTLINE));
	}
	CFTFont* getFont(uint32_t fontSize, int fontStyle, int outlineWidth)
	{
		return getFont(SFontKey(fontSize, fontStyle, outlineWidth));
	}
	CFTFont* getFont(const SFontKey& type)
	{
		auto itr = FontMap.find(type);
		if (itr != FontMap.end())
			return itr->second;

		return createFont(type);
	}

	static const int DEFAULT_FONT_STYLE = STYLE_NORMAL;
	static const int DEFAULT_FONT_OUTLINE = 0;

public:
	CFTFont* createFont(const SFontKey& type);
	void destroyFont(const SFontKey& type);
	void onWindowSizeChanged(const dimension2d& size);
	bool createDefaultFonts();

	const FT_Library&	 getFTLibrary() const { return FTLibrary; }
	const FTC_Manager& GetCacheManager() const { return FTCManager; }
	const FTC_CMapCache& GetCMapCache() const { return FTCCMapCache; }
	const FTC_ImageCache& GetImageCache() const { return FTCImageCache; }
	int GetCacheSizeMax() const { return CacheSizeMax; }

	My_FaceID LookupFaceID(const char *strFontPath, int nFaceIndex, int nFontStyle);
	void RemoveFaceID(My_FaceID faceId);

private:
	bool init(int nMaxFontFamilyCountInCache, int nMaxFontStyleInCache, int nMaxCacheSizeBytes);
	void release();

	void calcTotalFontTextureNum();

public:
	struct SFontDesc
	{
		std::string faceName;
		int faceIndex;
		uint32_t fontSize;
		int fontStyle;
		int outlineWidth;
	};

	struct FaceID_Key
	{
		std::string file_path;
		int face_index;
		unsigned int font_style;

		bool operator<(const FaceID_Key& other) const
		{
			if (file_path != other.file_path)
				return file_path < other.file_path;

			if (face_index != other.face_index)
				return face_index < other.face_index;

			if (font_style != other.font_style)
				return font_style < other.font_style;

			return false;
		}

		bool operator==(const FaceID_Key& other) const
		{
			return file_path == other.file_path &&
				face_index == other.face_index &&
				font_style == other.font_style;
		}
	};

	struct FaceID_Node
	{
		My_FaceID faceId;
		int refCount;

		FaceID_Node() : faceId(nullptr), refCount(0) {}
		FaceID_Node(My_FaceID faceId, int refCount) : faceId(faceId), refCount(refCount) {}
	};

	using T_FaceIDMap = std::map<FaceID_Key, FaceID_Node>;

private:
	FT_Library FTLibrary;
	FTC_Manager FTCManager;
	FTC_CMapCache FTCCMapCache;
	FTC_ImageCache FTCImageCache;
	int CacheSizeMax;
	int TotalFontTextureNum;

	T_FaceIDMap		m_FaceIDMap;

	using T_FontMap = std::map<SFontKey, CFTFont*>;
	//using T_FontMap = std::unordered_map<SFontKey, IFTFont*, SFontKey_hash>;

	T_FontMap		FontMap;
	const uint32_t		DefaultFontSize;
};
