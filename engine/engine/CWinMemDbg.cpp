#include "CWinMemDbg.h"
#include "CFileSystem.h"
#include "function.h"

size_t g_maxAlloc = 1000;
uint32_t g_BytesAllocated = 0;

#define nNoMansLandSize 4

typedef struct _CrtMemBlockHeader
{
	struct _CrtMemBlockHeader * pBlockHeaderNext;
	struct _CrtMemBlockHeader * pBlockHeaderPrev;
	char *                      szFileName;
	int                         nLine;
#ifdef _WIN64
	/* These items are reversed on Win64 to eliminate gaps in the struct
	* and ensure that sizeof(struct)%16 == 0, so 16-byte alignment is
	* maintained in the debug heap.
	*/
	int                         nBlockUse;
	size_t                      nDataSize;
#else  /* _WIN64 */
	size_t                      nDataSize;
	int                         nBlockUse;
#endif  /* _WIN64 */
	long                        lRequest;
	unsigned char               gap[nNoMansLandSize];
	/* followed by:
	*  unsigned char           data[nDataSize];
	*  unsigned char           anotherGap[nNoMansLandSize];
	*/
} _CrtMemBlockHeader;

#define pbData(pblock) ((unsigned char *)((_CrtMemBlockHeader *)pblock + 1))
#define pHdr(pbData) (((_CrtMemBlockHeader *)pbData)-1)

int __cdecl MyAllocHook(
	int      nAllocType,
	void   * pvData,
	size_t   nSize,
	int      nBlockUse,
	long     lRequest,
	const unsigned char * szFileName,
	int      nLine
	)
{
	const char *operation[] = { "", "allocating", "re-allocating", "freeing" };
	const char *blockType[] = { "Free", "Normal", "CRT", "Ignore", "Client" };

	if (nBlockUse == _CRT_BLOCK)   // Ignore internal C runtime library allocations
		return(TRUE);

	assert((nAllocType > 0) && (nAllocType < 4));
	assert((nBlockUse >= 0) && (nBlockUse < 5));

	if (nSize > g_maxAlloc && (nAllocType == 1 || nAllocType == 2))
	{
		/*
		ASys::OutputDebug("Memory operation in %s, line %d: %s a %d-byte '%s' block (#%ld)\n",
			szFileName, nLine, operation[nAllocType], nSize,
			blockType[nBlockUse], lRequest);
			*/
		g_BytesAllocated += (int)nSize;
	}

	return(TRUE);         // Allow the memory operation to proceed
}

void AWinMemDbg::beginCheckPoint()
{
	memset(m_BlockInfo, 0, sizeof(m_BlockInfo));

	_CrtMemCheckpoint(&OldState);
}

bool AWinMemDbg::endCheckPoint()
{
	_CrtMemCheckpoint(&NewState);

	int diff = _CrtMemDifference(&DiffState, &OldState, &NewState);
	return diff == 0;
}

void AWinMemDbg::outputDifference(const char* funcname)
{
	//ASys::OutputDebug("%s memory used: %0.2f M\n", funcname,
	//	DiffState.lSizes[_NORMAL_BLOCK] / 1048576.f);

	g_FileSystem->writeLog(ELOG_GX, "%s memory used: %d bytes\n", funcname,
		DiffState.lSizes[_NORMAL_BLOCK]);
}

void AWinMemDbg::setAllocHook(bool enable, int nMaxAlloc /*= 1000*/)
{
	if (enable)
	{
		_CrtSetAllocHook(MyAllocHook);
		g_maxAlloc = nMaxAlloc;
	}
	else
	{
		_CrtSetAllocHook(nullptr);
	}
}

void AWinMemDbg::outputMaxMemoryUsed()
{
	_CrtMemCheckpoint(&OldState);

	g_FileSystem->writeLog(ELOG_GX, "maximum memory used: %0.2f M\n", OldState.lHighWaterCount / 1048576.f);
}

const char* AWinMemDbg::getBlockAllocInfo(const void* address)
{
	_CrtMemBlockHeader* pHeader = NewState.pBlockHeader;
	while (pHeader != nullptr)
	{
		if (pHeader > pHdr(address))
		{
			sprintf(m_BlockInfo, "file: %s, line: %d", pHeader->szFileName, pHeader->nLine);
			return m_BlockInfo;
		}
		pHeader = pHeader->pBlockHeaderNext;
	}

	strcpy(m_BlockInfo, "nullptr");
	return m_BlockInfo;
}

void AWinMemDbg::registerFrame(TIME_POINT now)
{
	const uint32_t milli = CSysChrono::getDurationMilliseconds(now, LastTime);

	if (milli >= 2000)
	{
		const float invMilli = reciprocal_((float)milli);
		MPS = (1000 * (float)g_BytesAllocated) * invMilli;
		g_BytesAllocated = 0;
		LastTime = now;
	}
}
