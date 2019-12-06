#pragma once

#include <string>
#include <cstdint>
#include <vector>

enum {
	TID_LBRACKET = 0, // {
	TID_RBRACKET, // }
	TID_COLON, // :
	TID_VARIABLE, // $...
	TID_WORD, // *
	TID_QUOTE, // "*"
	TID_NEWLINE, // \n
	TID_UNKNOWN,
	TID_END
};

struct ScriptToken
{
	std::string lexeme;
	std::string file;
	uint32_t type;
	uint32_t line;
};

class ScriptLexer
{
public:
	std::vector<ScriptToken> tokenize(const char* str, const char* source);

private:
	void setToken(const char* lexeme, uint32_t line, const char* source, std::vector<ScriptToken>& tokens);
	bool isWhitespace(char c) const { return c == ' ' || c == '\r' || c == '\t'; }
	bool isNewline(char c) const { return c == '\n' || c == '\r'; }
};