#include <crtdbg.h>
#include <stdio.h>
#include <regex>
#include <iostream>
#include <list>

#include "CFileSystem.h"
#include "wowEnvironment.h"
#include "wowDatabase.h"
#include "function.h"

#include "wowDbFile.h"

using namespace std;

#pragma comment(lib, "CascLib.lib")
#pragma comment(lib, "pugixml.lib")

void testWowDatabase();

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	testWowDatabase();

	getchar();
	return 0;
}


void testWowDatabase()
{
	char workingDirectory[QMAX_PATH];
	Q_getcwd(workingDirectory, QMAX_PATH);

	CFileSystem* fs = new CFileSystem(workingDirectory, R"(E:\World Of Warcraft)");
	wowEnvironment* wowEnv = new wowEnvironment(fs);
	wowDatabase* wowDB = new wowDatabase(wowEnv);

	if (!wowEnv->init())
		printf("wowEnv init fail!\n");
	else
		printf("wowEnv init success!\n");

	if (!wowEnv->loadCascListFiles())
		printf("listfile fail!\n");
	else
		printf("listfile success!\n");

	if (!wowDB->init())
		printf("wowDB init fail!\n");
	else
		printf("wowDB init success!\n");

	for (const auto& kv : wowDB->getDBStructMap())
	{
		const CTableStruct& table = kv.second;
		const DBFile* file = wowDB->loadDBFile(&table);
		if (!file)
			printf("load table memFile fail! %s\n", table.name.c_str());
		else
			printf("%s\n", table.name.c_str());

		delete file;
	}
	

	delete wowDB;
	delete wowEnv;
	delete fs;
}