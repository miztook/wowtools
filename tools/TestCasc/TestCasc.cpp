#include <crtdbg.h>
#include <stdio.h>
#include <regex>
#include <iostream>

#include "CascLib.h"
#include "CascCommon.h"


#pragma comment(lib, "CascLib.lib")

static int TestOpenStorage_OpenFile(const TCHAR * szStorage, const char * szFileName);
static int TestOpenStorage_OpenFileByID(const TCHAR* szStorage, int fileId);
static int TestOpenStorage_EnumFiles(const TCHAR * szStorage, const TCHAR * szListFile = NULL);

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	/*
	const std::regex pattern("^(\\d).(\\d).(\\d).(\\d+)");
	std::match_results<std::string::const_iterator> sm;
	std::string str("8.1.5.29814");
	std::regex_match(str, sm, pattern);
	for (int i = 0; i < sm.size(); ++i)
		printf("%s\n", sm[i].str().c_str());
		*/

	const char* szStorage = R"(D:\World Of Warcraft 81\Data\)";
	//const char* filename = "Character\\HUMAN\\Male\\humanmale.m2";
	const char* filename = "Character\\ORC\\Male\\orcmale_hd.m2";
	//int err = TestOpenStorage_OpenFile(szStorage, filename);
	int err = TestOpenStorage_OpenFileByID(szStorage, 121287);

	if (err == ERROR_SUCCESS)
		printf("test succeed!\n");
	else
		printf("test failed!\n");

	printf("%s\n", szStorage);

	getchar();
	return 0;
}

static int TestOpenStorage_OpenFile(const TCHAR* szStorage, const char* szFileName)
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
	if (CascGetStorageInfo(hStorage, CascStorageTotalFileCount, &dwFileCount, sizeof(DWORD), NULL))
	{
		printf("file count: %d\n", dwFileCount);
	}

	DWORD dwFeatures = 0;
	if (CascGetStorageInfo(hStorage, CascStorageFeatures, &dwFeatures, sizeof(DWORD), NULL))
	{
		printf("has names? %s\n", (dwFeatures & CASC_FEATURE_FILE_NAMES) ? "YES" : "NO");
	}

	CASC_STORAGE_PRODUCT product;
	if (CascGetStorageInfo(hStorage, CascStorageProduct, &product, sizeof(CASC_STORAGE_PRODUCT), NULL))
	{
		printf("game build: %s, %d\n", product.szCodeName, product.BuildNumber);
	}

	if (nError == ERROR_SUCCESS)
	{
		//CascGetFileInfo

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

		// Read data from the file
		bool ret = CascReadFile(hFile, Buffer, sizeof(Buffer), &dwBytesRead);
		if (!ret)
			printf("file name : %s, file size: %d, read failed!\n", szFileName, dwFileSize);
		else
			printf("file name : %s, file size: %d, read success!\n", szFileName, dwFileSize);

		CascCloseFile(hFile);
	}

	// Close storage and return
	if (hStorage != NULL)
		CascCloseStorage(hStorage);
	return nError;
}

static int TestOpenStorage_OpenFileByID(const TCHAR* szStorage, int fileId)
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
	if (CascGetStorageInfo(hStorage, CascStorageTotalFileCount, &dwFileCount, sizeof(DWORD), NULL))
	{
		printf("file count: %d\n", dwFileCount);
	}

	DWORD dwFeatures = 0;
	if (CascGetStorageInfo(hStorage, CascStorageFeatures, &dwFeatures, sizeof(DWORD), NULL))
	{
		printf("has names? %s\n", (dwFeatures & CASC_FEATURE_FILE_NAMES) ? "YES" : "NO");
	}

	CASC_STORAGE_PRODUCT product;
	if (CascGetStorageInfo(hStorage, CascStorageProduct, &product, sizeof(CASC_STORAGE_PRODUCT), NULL))
	{
		printf("game build: %s, %d\n", product.szCodeName, product.BuildNumber);
	}

	if (nError == ERROR_SUCCESS)
	{
		//CascGetFileInfo

		// Open a file	
		if (!CascOpenFile(hStorage, CASC_FILE_DATA_ID(fileId), CASC_LOCALE_NONE, CASC_OPEN_BY_FILEID, &hFile))
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

		// Read data from the file
		bool ret = CascReadFile(hFile, Buffer, sizeof(Buffer), &dwBytesRead);
		if (!ret)
			printf("file name : %d, file size: %d, read failed!\n", fileId, dwFileSize);
		else
			printf("file name : %d, file size: %d, read success!\n", fileId, dwFileSize);

		CascCloseFile(hFile);
	}

	// Close storage and return
	if (hStorage != NULL)
		CascCloseStorage(hStorage);
	return nError;
}

static int TestOpenStorage_EnumFiles(const TCHAR * szStorage, const TCHAR* szListFile)
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