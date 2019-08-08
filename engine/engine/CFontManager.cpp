#include "CFontManager.h"
#include "CFTFont.h"


#define FONTSIZE_ROUND(x)  (((x) + 32) & ~63)
#define FONTSIZE_FLOOR(x) ((x) & ~63)
#define FONTSIZE_CEILING(x)  (((x) + 63) & ~63)

FT_Error MyFTCFaceRequest(FTC_FaceID  face_id,
	FT_Library  library,
	FT_Pointer  request_data,
	FT_Face*    aface)
{
	My_FaceID myFaceID = (My_FaceID)face_id;

	FT_Error error = 0;

	std::string strFullFontPath = myFaceID->file_path;

	error = FT_New_Face(
		library,
		strFullFontPath.c_str(),
		myFaceID->face_index,
		aface);

	if (!error)
		(*aface)->generic.data = (void*)UINT32_TO_PTR(myFaceID->font_style);
	return error;
}

CFontManager::CFontManager(int nMaxFontFamilyCountInCache, int nMaxFontStyleInCache, int nMaxCacheSizeBytes, uint32_t defaultFontSize)
	: DefaultFontSize(defaultFontSize)
{
	TotalFontTextureNum = 0;

	// init
	if (nMaxFontFamilyCountInCache == 0)
		nMaxFontFamilyCountInCache = 4;
	if (nMaxFontStyleInCache == 0)
		nMaxFontStyleInCache = 8;
	if (nMaxCacheSizeBytes == 0)
		nMaxCacheSizeBytes = 1024 * 1024 * 2;

	CacheSizeMax = nMaxCacheSizeBytes;

	init(nMaxFontFamilyCountInCache, nMaxFontStyleInCache, nMaxCacheSizeBytes);
}

CFontManager::~CFontManager()
{
	release();

	FTC_Manager_Done(FTCManager);

	FT_Done_FreeType(FTLibrary);
}

CFTFont* CFontManager::createFont(const SFontKey& type)
{
	T_FontMap::iterator itr = FontMap.find(type);
	if (itr != FontMap.end())
	{
		ASSERT(false);
		return nullptr;
	}

	CFTFont* pFont = new CFTFont(A_FACE_NAME,
		A_FACE_INDEX,
		type.size,
		type.style,
		type.outlineWidth);

	if (!pFont->init())
	{
		delete pFont;
		ASSERT(false && "CFontServices::createFont failed! font face");
		return nullptr;
	}

	FontMap[type] = pFont;

	calcTotalFontTextureNum();

	return pFont;
}

void CFontManager::destroyFont(const SFontKey& type)
{
	auto itr = FontMap.find(type);
	if (itr != FontMap.end())
	{
		delete itr->second;
		FontMap.erase(itr);
	}
}

bool CFontManager::init(int nMaxFontFamilyCountInCache, int nMaxFontStyleInCache, int nMaxCacheSizeBytes)
{
	FT_Error error;
	error = FT_Init_FreeType(&FTLibrary);
	if (error)
	{
		ASSERT(false);
		return false;
	}

	error = FTC_Manager_New(FTLibrary, nMaxFontFamilyCountInCache, nMaxFontStyleInCache, nMaxCacheSizeBytes, MyFTCFaceRequest, nullptr, &FTCManager);
	if (error)
	{
		ASSERT(false);
		return false;
	}

	error = FTC_CMapCache_New(FTCManager, &FTCCMapCache);
	if (error)
	{
		ASSERT(false);
		return false;
	}

	error = FTC_ImageCache_New(FTCManager, &FTCImageCache);
	if (error)
	{
		ASSERT(false);
		return false;
	}

	return true;
}

void CFontManager::release()
{
	for (auto itr = FontMap.begin(); itr != FontMap.end(); ++itr)
	{
		delete itr->second;
	}
	FontMap.clear();
}

void CFontManager::calcTotalFontTextureNum()
{
	int numTextures = 0;
	for (auto itr = FontMap.begin(); itr != FontMap.end(); ++itr)
	{
		CFTFont* ftFont = static_cast<CFTFont*>(itr->second);
		numTextures += (int)ftFont->getTextureCount();
	}

	if (numTextures > TotalFontTextureNum)
		TotalFontTextureNum = numTextures;
}

bool CFontManager::createDefaultFonts()
{
	CFTFont* pFont = createFont(SFontKey(DefaultFontSize, DEFAULT_FONT_STYLE, DEFAULT_FONT_OUTLINE));
	if (pFont)
		pFont->setFixedSize(true);
	return pFont != nullptr;
}

void CFontManager::onWindowSizeChanged(const dimension2d& size)
{
	//清除font重新建立，这样可以删除不用的字体，FixedSize除外

	for (auto itr = FontMap.begin(); itr != FontMap.end();)
	{
		CFTFont* pFont = itr->second;
		if (!pFont->isFixedSize())
		{
			FontMap.erase(itr++);
			delete pFont;
		}
		else
		{
			++itr;
		}
	}

	/*
	#ifdef A_2D_SCALABLE

	float scale = ASys::GetWindowScale();
	for (T_FontMap::iterator itr = FontMap.begin(); itr != FontMap.end(); ++itr)
	{
	SFontKey type = itr->first;
	CFTFont* pFont = static_cast<CFTFont*>(itr->second);

	uint32_t newsize = (uint32_t)(type.size * scale);
	if (pFont->getFontSize() != newsize)		//recreate
	{
	delete pFont;
	pFont  = new CFTFont(A_FACE_NAME,
	A_FACE_INDEX,
	(uint32_t)(type.size * scale),
	scale,
	type.style,
	type.outlineWidth);

	if (!pFont->init())
	{
	delete pFont;
	pFont = nullptr;
	}

	itr->second = pFont;
	}
	}

	#endif
	*/
}

My_FaceID CFontManager::LookupFaceID(const char* strFontPath, int nFaceIndex, int nFontStyle)
{
	FaceID_Key key;
	key.file_path = strFontPath;
	key.face_index = nFaceIndex;
	key.font_style = nFontStyle;

	ASSERT(!isAbsoluteFileName(strFontPath));

	T_FaceIDMap::iterator itr = m_FaceIDMap.find(key);
	if (itr != m_FaceIDMap.end())
	{
		FaceID_Node& node = itr->second;
		++node.refCount;
		return node.faceId;
	}

	My_FaceID faceId = new My_FaceID_Rec;
	strcpy(faceId->file_path, strFontPath);
	faceId->face_index = nFaceIndex;
	faceId->font_style = nFontStyle;

	m_FaceIDMap[key] = FaceID_Node(faceId, 1);
	return faceId;
}

void CFontManager::RemoveFaceID(My_FaceID faceId)
{
	if (faceId == nullptr)
		return;

	FaceID_Key key;
	key.file_path = faceId->file_path;
	key.face_index = faceId->face_index;
	key.font_style = faceId->font_style;

	T_FaceIDMap::iterator itr = m_FaceIDMap.find(key);
	if (itr != m_FaceIDMap.end())
	{
		FaceID_Node& node = itr->second;
		--node.refCount;

		if (node.refCount <= 0)
		{
			FTC_Manager_RemoveFaceID(FTCManager, (FTC_FaceID)node.faceId);
			delete node.faceId;
			m_FaceIDMap.erase(itr);
		}
	}
}

void CFontManager::flushAll2DText()
{
	for (auto itr = FontMap.begin(); itr != FontMap.end(); ++itr)
	{
		CFTFont* font = itr->second;
		font->flushText();
	}
}