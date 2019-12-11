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

#include "ScriptLexer.h"

void testLexer();

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	getchar();
	return 0;
}

void testLexer()
{
	char workingDirectory[QMAX_PATH];
	Q_getcwd(workingDirectory, QMAX_PATH);

	CFileSystem* fs = new CFileSystem(workingDirectory, R"(D:\World Of Warcraft 81)");

	//
	std::string dir = fs->getWorkingDirectory();
	normalizeDirName(dir);

	dir += "Shaders/";
	Q_MakeDirForFileName(dir.c_str());

	{
		CReadFile* rf = fs->createAndOpenFile((dir + "Redify.shader").c_str(), false);
		char* content = new char[rf->getSize()];
		rf->readText(content, rf->getSize());

		delete[] content;
		delete rf;
	}


	delete fs;
}