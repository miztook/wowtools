#pragma once

#include "base.h"
#include <unordered_map>
#include <map>
#include <list>

#include "FontDef.h"

#include <ft2build.h>
#include FT_GLYPH_H

class CFTFont;

typedef struct FT_BitmapGlyphRec_*  FT_BitmapGlyph;

struct SFTGlyphID
{
	int nIndex;
	bool bOutLine;

	bool operator<(const SFTGlyphID& other) const
	{
		if (nIndex != other.nIndex)
			return nIndex < other.nIndex;
		else
			return bOutLine < other.bOutLine;
	}

	bool operator==(const SFTGlyphID& other) const
	{
		return nIndex == other.nIndex && bOutLine == other.bOutLine;
	}
};


//维护freetype画glyph的一个缓存,
class CFTGlyphCacheNode
{
public:
	CFTGlyphCacheNode()
	{
		glyphID.nIndex = -1;
		glyphID.bOutLine = false;
		nSize = 0;
		glyph = nullptr;
	}

	~CFTGlyphCacheNode()
	{
		if (glyph)
		{
			FT_Done_Glyph((FT_Glyph)glyph);
			glyph = nullptr;
		}
	}

public:
	SFTGlyphID	glyphID;
	FT_BitmapGlyph glyph;
	int nSize;
};

class CFTGlyphCache
{
public:
	explicit CFTGlyphCache(CFTFont* font);
	virtual ~CFTGlyphCache();

public:
	FT_BitmapGlyph LookupGlyph(int nIndex, bool bRenderAsOutline);

private:
	void AddHead(CFTGlyphCacheNode* pNode);
	void RemoveTail();
	CFTGlyphCacheNode* NewNode(int nIndex, bool bRenderAsOutline);
	CFTGlyphCacheNode* FindInCache(int nIndex, bool bRenderAsOutline);
	void AddNode(CFTGlyphCacheNode* pNode);

	void ConvertToOutline(FT_BitmapGlyph ftGlyphOriginal, FT_BitmapGlyph *pFtGlyphDest);
	void ModifyGraphToFitOutlineStyleSize(FT_BitmapGlyph ftGlyphOriginal, FT_BitmapGlyph *pFtGlyphDest);

private:

	using T_GlyphCacheMap = std::map<SFTGlyphID, CFTGlyphCacheNode*>;
	//using T_GlyphCacheMap = std::unordered_map<SFTGlyphID, CFTGlyphCacheNode*, SFTGlyphID_hash>;

	T_GlyphCacheMap		m_GlyphCacheMap;
	std::list<CFTGlyphCacheNode*>	m_GlyphNodeList;
	int		m_nNodeCount;
	int		m_nNodeSizeSum;
	CFTFont*		m_FTFont;
};