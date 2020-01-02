#pragma once

#include <string>
#include <cstdint>
#include <vector>

enum : int 
{
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
	static std::vector<ScriptToken> tokenize(const char* str, const char* source, std::string& error);

private:
	static void setToken(const std::string& lexeme, uint32_t line, const char* source, std::vector<ScriptToken>& tokens);
	static bool isWhitespace(char c) { return c == ' ' || c == '\r' || c == '\t'; }
	static bool isNewline(char c) { return c == '\n' || c == '\r'; }
};