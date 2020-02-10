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
	static std::list<ConcreteNode*> parse(const std::vector<ScriptToken>& tokens);
	static std::list<ConcreteNode*> parseChunk(const std::vector<ScriptToken>& tokens);

	static const char* getTokenType(ConcreteNodeType type);

private:
	static std::vector<ScriptToken>::const_iterator skipNewLines(std::vector<ScriptToken>::const_iterator itr, std::vector<ScriptToken>::const_iterator end);
};

inline const char* ScriptParser::getTokenType(ConcreteNodeType type)
{
	switch (type)
	{
	case CNT_VARIABLE:
		return "Variable";
	case CNT_VARIABLE_ASSIGN:
		return "VaraibleAssign";
	case CNT_WORD:
		return "Word";
	case CNT_IMPORT:
		return "Import";
	case CNT_QUOTE:
		return "Quote";
	case CNT_LBRACE:
		return "LeftBrace";
	case CNT_RBRACE:
		return "RightBrace";
	case CNT_COLON:
		return "Colon";
	default:
		return "Unknown";
	}
}
