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
#include "ScriptCompiler.h"

#pragma comment(lib, "CascLib.lib")
#pragma comment(lib, "pugixml.lib")

void testLexer();
void testParser();
void printNode(const ConcreteNode* node);
void printNode(const AbstractNode* node);

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//_CrtSetBreakAlloc(715);

	//testLexer();
	testParser();

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

	const char* files[] =
	{
		//"test.shader",
		//"Redify.shader",
		"UI.material",
	};

	for (int i = 0; i < ARRAY_COUNT(files); ++i)
	{
		std::string filename = dir + files[i];
		CReadFile* rf = fs->createAndOpenFile(filename.c_str(), false);
		uint32_t len = rf->getSize();
		char* content = new char[len];
		memset(content, 0, len);
		rf->read(content, len);

		//read lexer
		std::string error;
		std::vector<ScriptToken> tokenList = ScriptLexer::tokenize(content, filename.c_str(), error);
		if (error.empty())
		{
			for (const auto& token : tokenList)
			{
				printf("token type: %s, %s, %s, %d\n", 
					getLexerTokenType(token.type), token.lexeme.c_str(), getFileNameA(token.file.c_str()).c_str(), token.line);
			}
		}

		delete[] content;
		delete rf;
	}


	delete fs;
}

void testParser()
{
	CFileSystem* fs = new CFileSystem(R"(D:\World Of Warcraft 81)");

	//
	std::string dir = fs->getWorkingDirectory();
	normalizeDirName(dir);

	dir += "Shaders/";
	Q_MakeDirForFileName(dir.c_str());

	const char* files[] = 
	{
		//"test.shader",
		//"Redify.shader",
		"UI.material",
	};

	for (int i = 0; i < ARRAY_COUNT(files); ++i)
	{
		std::string filename = dir + files[i];
		CReadFile* rf = fs->createAndOpenFile(filename.c_str(), false);
		uint32_t len = rf->getSize();
		char* content = new char[len];
		memset(content, 0, len);
		rf->read(content, len);

		//read lexer
		std::string error;
		std::vector<ScriptToken> tokenList = ScriptLexer::tokenize(content, filename.c_str(), error);
		ASSERT(error.empty());
		std::list<ConcreteNode*> nodes = ScriptParser::parse(tokenList);

		for (const ConcreteNode* node : nodes)
		{
			printNode(node);
		}

		for (ConcreteNode* node : nodes)
		{
			ConcreteNode::deleteNode(node);
		}
		nodes.clear();

		delete[] content;
		delete rf;
	}

	delete fs;
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
	printf("%s token: %s, type: %s, line: %d\n", str.c_str(), node->token.c_str(), getConcreateNodeType(node->type), node->line);

	for (const ConcreteNode* child : node->children)
	{
		printNode(child);
	}
}

void printNode(const AbstractNode* node)
{
	int n = 0;
	const AbstractNode* p = node->parent;
	std::string str;
	while (p)
	{
		++n;
		str += "\t";
		p = p->parent;
	}

	printf("%s type: %s, name: %s, line: %d\n", str.c_str(), getAstNodeType(node->type), node->getValue(), node->line);

	for (const AbstractNode* child : node->children)
	{
		printNode(child);
	}
}
