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

void testCompiler();
void printNode(const AbstractNode* node);

int main(int argc, char* argv[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//_CrtSetBreakAlloc(1725);

	testCompiler();
	
	getchar();
	return 0;
}

void testCompiler()
{
	CFileSystem* fs = new CFileSystem(R"(D:\World Of Warcraft 81)");

	//
	std::string dir = fs->getWorkingDirectory();
	normalizeDirName(dir);

	dir += "Shaders/";
	Q_MakeDirForFileName(dir.c_str());

	const char* files[] =
	{
		//"Redify.shader",
		"UI.material",
	};

	MaterialCompiler mgr;

	for (int i = 0; i < ARRAY_COUNT(files); ++i)
	{
		std::string filename = dir + files[i];
		CReadFile* rf = fs->createAndOpenFile(filename.c_str(), false);
		uint32_t len = rf->getSize();
		char* content = new char[len];
		memset(content, 0, len);
		rf->read(content, len);

		//mgr.parseScript(content, filename.c_str());

		/*
		std::string error;
		std::vector<ScriptToken> tokenList = ScriptLexer::tokenize(content, files[i], error);
		ASSERT(error.empty());
		std::list<ConcreteNode*> nodes = ScriptParser::parse(tokenList);
		
		//to ast
		std::list<AbstractNode*> astNodes = mgr.convertToAST(nodes);
		for (const AbstractNode* n : astNodes)
		{
			printNode(n);
		}

		for (AbstractNode* n : astNodes)
			AbstractNode::deleteNode(n);

		for (ConcreteNode* n : nodes)
			ConcreteNode::deleteNode(n);
		*/

		delete[] content;
		delete rf;
	}


	delete fs;
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

	if (node->type == ANT_OBJECT)
	{
		const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);
		for (const AbstractNode* child : obj->children)
		{
			printNode(child);
		}
	}
}
