#include <iostream>
#include <list>

#include "CFileSystem.h"
#include "wowEnvironment.h"
#include "wowDatabase.h"
#include "function.h"

#include "wowDbFile.h"
#include "wowWMOFile.h"

#pragma comment(lib, "CascLib.lib")
#pragma comment(lib, "pugixml.lib")

void testWowGameFile();

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	testWowGameFile();

	getchar();
	return 0;
}

void testWowGameFile()
{
	char		workingDirectory[QMAX_PATH];
	Q_getcwd(workingDirectory, QMAX_PATH);

	CFileSystem* fs = new CFileSystem(workingDirectory, R"(E:\World Of Warcraft)");
	wowEnvironment* wowEnv = new wowEnvironment(fs);

	if (!wowEnv->init())
		printf("init fail!\n");
	else
		printf("init success!\n");

	if (!wowEnv->loadCascListFiles())
		printf("listfile fail!\n");
	else
		printf("listfile success!\n");

	wowEnv->buildWmoFileList();

	const char* path = "World\\wmo\\Northrend\\Dalaran\\ND_Dalaran.wmo";
	CMemFile* memFile = wowEnv->openFile(path);
	wowWMOFile* wmoFile = new wowWMOFile();
	wmoFile->loadFile(memFile);
	delete wmoFile;
	delete memFile;

	delete wowEnv;
	delete fs;
}