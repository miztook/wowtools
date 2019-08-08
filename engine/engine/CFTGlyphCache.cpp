#include "CFTGlyphCache.h"
#include "CFontManager.h"
#include "CFTFont.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_RENDER_H
#include FT_OUTLINE_H

CFTGlyphCache::CFTGlyphCache(CFTFont* font)
	: m_FTFont(font)
{
	m_nNodeCount = 0;
	m_nNodeSizeSum = 0;
}

CFTGlyphCache::~CFTGlyphCache()
{
	for (auto& node : m_GlyphNodeList)
	{
		delete node;
	}
	m_GlyphNodeList.clear();
}

FT_BitmapGlyph CFTGlyphCache::LookupGlyph(int nIndex, bool bRenderAsOutline)
{
	CFTGlyphCacheNode *pNode = FindInCache(nIndex, bRenderAsOutline && m_FTFont->getOutlineWidth() > 0);
	if (!pNode)
		pNode = NewNode(nIndex, bRenderAsOutline);

	if (!pNode)
		return nullptr;
	else
		return pNode->glyph;
}

void CFTGlyphCache::AddHead(CFTGlyphCacheNode* pNode)
{
	m_GlyphNodeList.push_back(pNode);

	m_GlyphCacheMap[pNode->glyphID] = pNode;

	++m_nNodeCount;
	m_nNodeSizeSum += pNode->nSize;
}

void CFTGlyphCache::RemoveTail()
{
	if (m_GlyphNodeList.empty())
		return;

	CFTGlyphCacheNode* node = m_GlyphNodeList.back();
	m_GlyphNodeList.pop_back();

	--m_nNodeCount;
	m_nNodeSizeSum -= node->nSize;

	T_GlyphCacheMap::iterator itr = m_GlyphCacheMap.find(node->glyphID);
	if (itr != m_GlyphCacheMap.end())
	{
		m_GlyphCacheMap.erase(itr);
	}

	delete node;
}

CFTGlyphCacheNode * CFTGlyphCache::NewNode(int nIndex, bool bRenderAsOutline)
{
	const CFontManager* fontManager = m_FTFont->getFontManager();

	FT_Glyph ftResultGlyph = nullptr;
	FT_Error error = 0;

	bool bCreateAsOutline = bRenderAsOutline && m_FTFont->getOutlineWidth() > 0;
	if (bCreateAsOutline)
	{
		ASSERT_TODO
	}
	else
	{
		// get outline
		FT_Size ftSize = m_FTFont->GetFTSize();
		FT_Face ftFace = ftSize->face;

		// by Silas - using FreeType Glyph Cache.
		FT_Glyph rawGlyph;
		FTC_Node rawNode;
		FTC_ImageTypeRec imageDesc;
		imageDesc.face_id = m_FTFont->GetMyFaceID();
		imageDesc.width = ftSize->metrics.x_ppem;//0;
		imageDesc.height = ftSize->metrics.y_ppem;//TheFont->GetFTCScaler()->height >> 6;
		imageDesc.flags = FT_LOAD_NO_BITMAP | /*FT_LOAD_NO_HINTING |*/ FT_LOAD_NO_AUTOHINT;
		error = FTC_ImageCache_Lookup(fontManager->GetImageCache(), &imageDesc, nIndex, &rawGlyph, &rawNode);
		if (rawGlyph->format == FT_GLYPH_FORMAT_OUTLINE)
		{
			FT_Glyph_Copy(rawGlyph, &ftResultGlyph);

			unsigned int fontStyle = PTR_TO_UINT32(ftFace->generic.data);

			if (fontStyle & STYLE_BOLD)
			{
				FT_OutlineGlyph outlineGlyph = (FT_OutlineGlyph)ftResultGlyph;
				FT_Pos      xstr;
				xstr = m_FTFont->GetBoldWidth();
				FT_Outline_Embolden(&outlineGlyph->outline, xstr);
			}
			if (fontStyle & STYLE_ITALIC)
			{
				FT_Matrix    transform;
				transform.xx = 0x10000L;
				transform.yx = 0x00000L;

				transform.xy = 0x0366AL;
				transform.yy = 0x10000L;

				FT_Glyph_Transform(ftResultGlyph, &transform, nullptr);
			}
			error = FT_Glyph_To_Bitmap(&ftResultGlyph, FT_RENDER_MODE_NORMAL, nullptr, 1);
		}
		FTC_Node_Unref(rawNode, fontManager->GetCacheManager());
		if (ftResultGlyph == nullptr)
			return nullptr;

		if (m_FTFont->getOutlineWidth() > 0)
		{
			FT_BitmapGlyph glyph;
			ModifyGraphToFitOutlineStyleSize((FT_BitmapGlyph)ftResultGlyph, (FT_BitmapGlyph *)&glyph);
			FT_Done_Glyph(ftResultGlyph);
			ftResultGlyph = (FT_Glyph)glyph;
		}
	}

	//创建node
	CFTGlyphCacheNode* pNode = new CFTGlyphCacheNode;
	pNode->glyphID = { nIndex, bRenderAsOutline };
	FT_BitmapGlyph bitmapGlyph = (FT_BitmapGlyph)ftResultGlyph;
	pNode->glyph = bitmapGlyph;
	pNode->nSize +=
		sizeof(bitmapGlyph) +
		sizeof(bitmapGlyph->bitmap) +
		bitmapGlyph->bitmap.rows * bitmapGlyph->bitmap.pitch;

	AddNode(pNode);

	return pNode;
}

void CFTGlyphCache::AddNode(CFTGlyphCacheNode* pNode)
{
	const CFontManager* fontManager = m_FTFont->getFontManager();

	if (m_GlyphNodeList.empty() &&
		m_nNodeSizeSum + pNode->nSize > fontManager->GetCacheSizeMax())			//full
	{
		RemoveTail();
	}

	AddHead(pNode);
}

void CFTGlyphCache::ModifyGraphToFitOutlineStyleSize(FT_BitmapGlyph ftGlyphOriginal, FT_BitmapGlyph *pFtGlyphDest)
{
	ASSERT_TODO
}

CFTGlyphCacheNode * CFTGlyphCache::FindInCache(int nIndex, bool bRenderAsOutline)
{
	SFTGlyphID ID = { nIndex, bRenderAsOutline };
	T_GlyphCacheMap::iterator itr = m_GlyphCacheMap.find(ID);
	if (itr == m_GlyphCacheMap.end())
		return nullptr;

	CFTGlyphCacheNode* p = itr->second;
	if (p)
	{
		if (m_nNodeCount > 1)		//move to head
		{
			m_GlyphNodeList.remove(p);
			m_GlyphNodeList.push_front(p);
		}
	}

	return p;
}