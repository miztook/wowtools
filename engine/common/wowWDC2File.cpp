#include "wowWDC2File.h"
#include "CMemFile.h"
#include "wowDatabase.h"
#include "stringext.h"
#include <cassert>

WDC2File::WDC2File(CMemFile * memFile)
	: DBFile(memFile), m_isSparseTable(false)
{
	memset(&m_header, 0, sizeof(m_header));
}