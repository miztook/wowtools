#include <crtdbg.h>
#include <stdio.h>
#include <regex>
#include <iostream>
#include <set>

#include "CFileSystem.h"
#include "wowEnvironment.h"
#include "function.h"
#include "CSysChrono.h"
#include "stringext.h"

#pragma comment(lib, "CascLib.lib")
#pragma comment(lib, "pugixml.lib")

void testWowEnvironment81();
void testWowEnvironmentClassic();

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//testWowEnvironment81();
	testWowEnvironmentClassic();

	getchar();
	return 0;
}

void testWowEnvironment81()
{
	CFileSystem* fs = new CFileSystem(R"(E:\World Of Warcraft)");
	wowEnvironment* wowEnv = new wowEnvironment(fs);

	if (!wowEnv->init("wow"))
		printf("init fail!\n");
	else
		printf("init success!\n");

	TIME_POINT last = CSysChrono::getTimePointNow();

	if (!wowEnv->loadCascListFiles())
		printf("listfile fail!\n");
	else
		printf("listfile success!\n");

	uint32_t ms = CSysChrono::getDurationMilliseconds(last);
	printf("duration: %u\n", ms);

	delete wowEnv;
	delete fs;
}

void testWowEnvironmentClassic()
{
	CFileSystem* fs = new CFileSystem(R"(E:\World Of Warcraft)");
	wowEnvironment* wowEnv = new wowEnvironment(fs);

	if (!wowEnv->init("wow_classic"))
		printf("init fail!\n");
	else
		printf("init success!\n");

	/*
	std::set<uint32_t> fileDataIdSet;
	wowEnv->iterateAllFileId([&fileDataIdSet](uint32_t fileDataId) { fileDataIdSet.insert(fileDataId); });

	std::string dir = fs->getDataDirectory();
	normalizeDirName(dir);
	dir += "1.13";
	normalizeDirName(dir);

	std::string listFile = dir + "listfile.csv";
	std::string outlistFile = dir + "newlistfile.csv";

	CWriteFile* wfile = fs->createAndWriteFile(outlistFile.c_str(), false);
	if (wfile)
	{
		CReadFile* file = fs->createAndOpenFile(listFile.c_str(), false);
		if (file)
		{
			char buffer[1024] = { 0 };
			std::string strLine;
			while (file->readLine(buffer, 1024))
			{
				char* p = strstr(buffer, ";");
				if (!p)
					continue;

				strLine = buffer;

				std::string filename(p + 1);
				normalizeFileName(filename);
				str_tolower(filename);

				*p = '\0';

				int id = atoi(buffer);
				if (id >= 0 && fileDataIdSet.find(id) != fileDataIdSet.end())
				{
					wfile->writeLine("%s", strLine.c_str());
				}
			}
			delete file;
		}
		delete wfile;
	}
	*/

	delete wowEnv;
	delete fs;
}