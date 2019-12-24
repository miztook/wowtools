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
#include "ScriptParser.h"

#pragma comment(lib, "CascLib.lib")
#pragma comment(lib, "pugixml.lib")

void testParser();
void printNode(const ConcreteNode* node);

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	testParser();

	getchar();
	return 0;
}

void testParser()
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
		std::list<ConcreteNode*> nodes = ScriptParser::parse(tokenList);

		for (const ConcreteNode* node : nodes)
		{
			printNode(node);
		}

		delete[] content;
		delete rf;
	}
}

void printNode(const ConcreteNode* node)
{
	int n = 0;
	const ConcreteNode* p = node->parent;
	std::string str;
	while (p)
	{
		++n;
		str += "\t";
		p = p->parent;
	}
	printf("%s token: %s, type: %d, line: %d\n", str.c_str(), node->token.c_str(), node->type, node->line);

	for (const ConcreteNode* child : node->children)
	{
		printNode(child);
	}
}

