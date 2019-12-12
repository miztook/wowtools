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
	ConcreteNode() : parent(nullptr), line(0) {}

	std::string token;
	std::string file;
	uint32_t line;
	ConcreteNodeType type;
	std::list<ConcreteNode*> children;
	ConcreteNode* parent;
};

class ScriptParser
{
public:
	virtual ~ScriptParser() = default;

public:
	std::list<ConcreteNode*> parse(const std::vector<ScriptToken>& tokens);
	std::list<ConcreteNode*> parseChunk(const std::vector<ScriptToken>& tokens);

private:
	std::vector<ScriptToken>::const_iterator skipNewLines(std::vector<ScriptToken>::const_iterator itr, std::vector<ScriptToken>::const_iterator end);
};