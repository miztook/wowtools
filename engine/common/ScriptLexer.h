#pragma once

#include <string>
#include <cstdint>
#include <vector>

enum LexerTokenType : int
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

inline const char* getLexerTokenType(int type)
{
	switch (type)
	{
	case TID_LBRACKET:
		return "LeftBracket";
	case TID_RBRACKET:
		return "RightBracket";
	case TID_COLON:
		return "Colon";
	case TID_VARIABLE:
		return "Variable";
	case TID_WORD:
		return "Word";
	case TID_QUOTE:
		return "Quote";
	case TID_NEWLINE:
		return "NewLine";
	case TID_UNKNOWN:
	default:
		return "unknown";
	}
}


struct ScriptToken
{
	std::string lexeme;
	std::string file;
	int type;
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

