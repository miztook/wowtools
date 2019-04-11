#include <crtdbg.h>
#include <stdio.h>
#include <regex>
#include <iostream>

#include "CascLib.h"
#include "CascCommon.h"

#pragma comment(lib, "CascLib.lib")

static int TestOpenStorage_OpenFile(const TCHAR * szStorage, const char * szFileName);
static int TestOpenStorage_EnumFiles(const TCHAR * szStorage, const TCHAR * szListFile = NULL);

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	const std::regex pattern("^(\\d).(\\d).(\\d).(\\d+)");
	std::match_results<std::string::const_iterator> sm;
	std::string str("8.1.5.29814");
	std::regex_match(str, sm, pattern);
	for (int i = 0; i < sm.size(); ++i)
		printf("%s\n", sm[i].str().c_str());

	const char* szStorage = R"(D:\World Of Warcraft\Data\)";
	const char* filename = "character/human/male/humanmale00.skin";
	int err = TestOpenStorage_OpenFile(szStorage, filename);

	if (err == ERROR_SUCCESS)
		printf("test succeed!\n");
	else
		printf("test failed!\n");

	printf("%s\n", szStorage);

	getchar();
	return 0;
}

static int TestOpenStorage_OpenFile(const TCHAR * szStorage, const char * szFileName)
{
	HANDLE hStorage;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD dwBytesRead;
	BYTE Buffer[0x1000];
	int nError = ERROR_SUCCESS;

	// Open the storage directory
	if (!CascOpenStorage(szStorage, 0, &hStorage))
	{
		assert(GetLastError() != ERROR_SUCCESS);
		nError = GetLastError();
	}

	DWORD dwFileCount = 0;
	if (CascGetStorageInfo(hStorage, CascStorageFileCount, &dwFileCount, sizeof(DWORD), NULL))
	{
		printf("file count: %d\n", dwFileCount);
	}

	DWORD dwFeatures = 0;
	if (CascGetStorageInfo(hStorage, CascStorageFeatures, &dwFeatures, sizeof(DWORD), NULL))
	{
		printf("has names? %s\n", (dwFeatures & CASC_FEATURE_HAS_NAMES) ? "YES" : "NO");
	}

	DWORD dwGameBuild = 0;
	if (CascGetStorageInfo(hStorage, CascStorageGameBuild, &dwGameBuild, sizeof(DWORD), NULL))
	{
		printf("game build: %d\n", dwGameBuild);
	}

	if (nError == ERROR_SUCCESS)
	{
		int id = CascGetFileId(hStorage, szFileName);
		printf("file name : %s, file id : %d\n", szFileName, id);

		// Open a file	
		if (!CascOpenFile(hStorage, szFileName, CASC_LOCALE_ZHCN, 0, &hFile))
		{
			assert(GetLastError() != ERROR_SUCCESS);
			nError = GetLastError();
		}
	}

	// Read some data from the file
	if (nError == ERROR_SUCCESS)
	{
		DWORD dwFileSize, dwFileSizeHigh;
		dwFileSize = CascGetFileSize(hFile, &dwFileSizeHigh);
		printf("file name : %s, file size: %d\n", szFileName, dwFileSize);

		// Read data from the file
		CascReadFile(hFile, Buffer, sizeof(Buffer), &dwBytesRead);
		CascCloseFile(hFile);
	}

	// Close storage and return
	if (hStorage != NULL)
		CascCloseStorage(hStorage);
	return nError;
}

static int TestOpenStorage_EnumFiles(const TCHAR * szStorage, const TCHAR * szListFile)
{
	CASC_FIND_DATA FindData;
	HANDLE hStorage;
	HANDLE hFind;
	bool bFileFound = true;
	int nError = ERROR_SUCCESS;

	// Open the storage directory
	if (!CascOpenStorage(szStorage, 0, &hStorage))
	{
		assert(GetLastError() != ERROR_SUCCESS);
		nError = GetLastError();
	}

	if (nError == ERROR_SUCCESS)
	{
		hFind = CascFindFirstFile(hStorage, "*", &FindData, szListFile);
		if (hFind != NULL)
		{
			while (bFileFound)
			{
				// Extract the file
				printf("%s\n", FindData.szFileName);

				// Find the next file in CASC
				bFileFound = CascFindNextFile(hFind, &FindData);
			}

			// Just a testing call - must fail
			CascFindNextFile(hFind, &FindData);

			// Close the search handle
			CascFindClose(hFind);
		}
	}

	// Close storage and return
	if (hStorage != NULL)
		CascCloseStorage(hStorage);
	return nError;
}