#pragma once

#include "ScriptLexer.h"

#include <vector>
#include <list>

enum ConcreteNodeType : int
{
	CNT_VARIABLE,
	CNT_VARIABLE_ASSIGN,
	CNT_WORD,
	CNT_IMPORT,
	CNT_QUOTE,
	CNT_LBRACE,
	CNT_RBRACE,
	CNT_COLON
};

struct ConcreteNode
{
	std::string token;
	std::string file;
	ConcreteNodeType type;
	std::list<ConcreteNode*> children;
	ConcreteNode* parent;
};

class ScriptParser
{
public:
	virtual ~ScriptParser() = default;

public:
	std::list<ConcreteNode> parse(const std::vector<ScriptToken>& tokens);
	std::list<ConcreteNode> parseChunk(const std::vector<ScriptToken>& tokens);

private:
	std::vector<ScriptToken>::iterator skipNewLines(std::vector<ScriptToken>::iterator itr, std::vector<ScriptToken>::iterator end);
};