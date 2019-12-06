#include "ScriptLexer.h"

void ScriptLexer::setToken(const char* lexeme, uint32_t line, const char* source, std::vector<ScriptToken>& tokens)
{
	const char openBracket = '{';
	const char closeBracket = '}';
	const char colon = ':';
	const char quote = '\"';
	const char var = '$';

	bool ignore = false;

	ScriptToken token;
	token.lexeme = lexeme;
	token.line = line;
	token.file = source;

	const auto len = strlen(lexeme);

	if (len == 1 && isNewline(lexeme[0]))
	{
		token.type = TID_NEWLINE;
		if (!tokens.empty() && tokens.back().type == TID_NEWLINE)
			ignore = true;
	}
	else if (len == 1 && lexeme[0] == openBracket)
		token.type = TID_LBRACKET;
	else if (len == 1 && lexeme[0] == closeBracket)
		token.type = TID_RBRACKET;
	else if (len == 1 && lexeme[0] == colon)
		token.type = TID_COLON;
	else if (lexeme[0] == var)
		token.type = TID_VARIABLE;
	else
	{
		// This is either a non-zero length phrase or quoted phrase
		if (len >= 2 && lexeme[0] == quote && lexeme[len - 1] == quote)
		{
			token.type = TID_QUOTE;
		}
		else
		{
			token.type = TID_WORD;
		}
	}

	if (!ignore)
		tokens.push_back(token);
}
