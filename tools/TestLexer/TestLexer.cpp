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

#pragma comment(lib, "CascLib.lib")
#pragma comment(lib, "pugixml.lib")

void testLexer();

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	testLexer();

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

		//read lexer
		std::string error;
		std::vector<ScriptToken> tokenList = ScriptLexer::tokenize(content, "Redify.shader", error);
		if (error.empty())
		{
			for (const auto& token : tokenList)
			{
				printf("token type: %d, %s, %s, %d", token.type, token.lexeme.c_str(), token.file.c_str(), token.line);
			}
		}

		delete[] content;
		delete rf;
	}


	delete fs;
}