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

	CFileSystem* fs = new CFileSystem(R"(D:\World Of Warcraft 81)");

	//
	std::string dir = fs->getWorkingDirectory();
	normalizeDirName(dir);

	dir += "Shaders/";
	Q_MakeDirForFileName(dir.c_str());

	{
		const char* filename = "test.shader";
		CReadFile* rf = fs->createAndOpenFile((dir + filename).c_str(), false);
		uint32_t len = rf->getSize();
		char* content = new char[len];
		memset(content, 0, len);
		rf->read(content, len);

		//read lexer
		std::string error;
		std::vector<ScriptToken> tokenList = ScriptLexer::tokenize(content, filename, error);
		if (error.empty())
		{
			for (const auto& token : tokenList)
			{
				printf("token type: %d, %s, %s, %d\n", token.type, token.lexeme.c_str(), token.file.c_str(), token.line);
			}
		}

		delete[] content;
		delete rf;
	}


	delete fs;
}