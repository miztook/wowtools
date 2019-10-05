#include <crtdbg.h>
#include <stdio.h>
#include <regex>
#include <iostream>
#include <list>

#include "CFileSystem.h"
#include "wowEnvironment.h"
#include "wowDatabase.h"
#include "function.h"
#include "stringext.h"

#include "wowDbFile.h"

#pragma comment(lib, "CascLib.lib")
#pragma comment(lib, "pugixml.lib")

void testWowDatabase();
void dumpWowDatabase(CFileSystem* fs, const wowDatabase* wowDB);

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

	CFileSystem* fs = new CFileSystem(workingDirectory, R"(D:\World Of Warcraft)");
	wowEnvironment* wowEnv = new wowEnvironment(fs);
	wowDatabase* wowDB = new wowDatabase(wowEnv);

	if (!wowEnv->init())
		printf("wowEnv init fail!\n");
	else
		printf("wowEnv init success!\n");

		/*
	if (!wowEnv->loadCascListFiles())
		printf("listfile fail!\n");
	else
		printf("listfile success!\n");
		*/

	if (!wowDB->init())
	{
		printf("wowDB init fail!\n");
	}
	else
	{
		printf("wowDB init success!\n");
		dumpWowDatabase(fs, wowDB);
	}
	
	delete wowDB;
	delete wowEnv;
	delete fs;
}

void dumpWowDatabase(CFileSystem* fs, const wowDatabase* wowDB)
{
	std::string dir = fs->getWorkingDirectory();
	normalizeDirName(dir);

	{
		CWriteFile* wf = fs->createAndWriteFile((dir + "AnimationData.txt").c_str(), false);

		const auto& records = wowDB->m_AnimationDataTable.RecordList;
		wf->writeLine("AnimationData: %d", (int)records.size());
		for (const auto& r : records)
		{
			wf->writeLine("AnimationData ID: %u, Name: %s",
				r.ID, r.Name.c_str());
		}
		delete wf;
	}
}