#include "ScriptLexer.h"
#include "stringext.h"

std::vector<ScriptToken> ScriptLexer::tokenize(const char* str, const char* source, std::string& error)
{
	enum { READY = 0, COMMENT, MULTICOMMENT, WORD, QUOTE, VAR, POSSIBLECOMMENT };

	const char varopener = '$';
	const char quote = '\"';
	const char slash = '/';
	const char backslash = '\\';
	const char openbrace = '{';
	const char closebrace = '}';
	const char colon = ':';
	const char star = '*';
	const char cr = '\r';
	const char lf = '\n';

	char c = 0, lastc = 0;

	std::string lexeme;
	uint32_t line = 1, state = READY, lastQuote = 0, firstOpenBrace = 0, braceLayer = 0;

	std::vector<ScriptToken> tokens;

	for (size_t i = 0; i < strlen(str); ++i)
	{
		lastc = c;
		c = str[i];

		if (c == quote)
			lastQuote = line;

		if (state == READY || state == WORD || state == VAR)
		{
			if (c == openbrace)
			{
				if (braceLayer == 0)
					firstOpenBrace = line;

				++braceLayer;
			}
			else if (c == closebrace)
			{
				if (braceLayer == 0)
				{
					error = std_string_format("no matching open bracket '{' found for close bracket '}' at %s:%d", source, line);
					return tokens;
				}

				--braceLayer;
			}
		}

		switch (state)
		{
		case READY:
			if (c == slash && lastc == slash)			//	//
			{
				lexeme = "";
				state = COMMENT;
			}
			else if (c == star && lastc == slash)		//	/*
			{
				lexeme = "";
				state = MULTICOMMENT;
			}
			else if (c == quote)						//	"
			{
				lexeme = c;
				state = QUOTE;
			}
			else if (c == varopener)					//	$
			{
				lexeme = c;
				state = VAR;
			}
			else if (isNewline(c))
			{
				lexeme = c;
				setToken(lexeme, line, source, tokens);
			}
			else if (!isWhitespace(c))
			{
				lexeme = c;
				if (c == slash)
					state = POSSIBLECOMMENT;			//	ÊäÈë/ºó¿ÉÄÜÊÇ×¢ÊÍ
				else
					state = WORD;
			}
			break;
		case COMMENT:
			if (isNewline(c))
			{
				lexeme = c;
				setToken(lexeme, line, source, tokens);
				state = READY;
			}
			break;
		case MULTICOMMENT:
			if (c == slash && lastc == star)			//  */
			{
				state = READY;
			}
			break;
		case POSSIBLECOMMENT:
			if (c == slash && lastc == slash)			//	//
			{
				lexeme = "";
				state = COMMENT;
				break;
			}
			else if (c == star && lastc == slash)		//	/*
			{
				lexeme = "";
				state = MULTICOMMENT;
				break;
			}
			else
			{
				state = WORD;				//continue WORD
			}
		case WORD:
			if (isNewline(c))				//»»ÐÐ½áÊø
			{
				setToken(lexeme, line, source, tokens);
				lexeme = c;
				setToken(lexeme, line, source, tokens);
				state = READY;
			}
			else if (isWhitespace(c))		//¿Õ¸ñ
			{
				setToken(lexeme, line, source, tokens);
				state = READY;
			}
			else if (c == openbrace || c == closebrace || c == colon)	//À¨ºÅ
			{
				setToken(lexeme, line, source, tokens);
				lexeme = c;
				setToken(lexeme, line, source, tokens);
				state = READY;
			}
			else
			{
				lexeme += c;
			}
			break;
		case QUOTE:
			if (c != backslash)
			{
				if (c == quote && lastc == backslash)		//	\"
				{
					lexeme += c;
				}
				else if (c == quote)						//	"
				{
					lexeme += c;
					setToken(lexeme, line, source, tokens);
					state = READY;
				}
				else
				{
					if (lastc == backslash)
						lexeme = lexeme + "\\" + c;
					else
						lexeme += c;
				}
			}
			break;
		case VAR:
			if (isNewline(c))
			{
				setToken(lexeme, line, source, tokens);
				lexeme = c;
				setToken(lexeme, line, source, tokens);
				state = READY;
			}
			else if (isWhitespace(c))
			{
				setToken(lexeme, line, source, tokens);
				state = READY;
			}
			else if (c == openbrace || c == closebrace || c == colon)
			{
				setToken(lexeme, line, source, tokens);
				lexeme = c;
				setToken(lexeme, line, source, tokens);
				state = READY;
			}
			else
			{
				lexeme += c;
			}
			break;
		}

		if (c == cr || (c == lf && lastc != cr))
			++line;
	}//for

	//¼ì²éÍË³ö×´Ì¬
	if (state == WORD || state == VAR)
	{
		if (!lexeme.empty())
			setToken(lexeme, line, source, tokens);
	}
	else
	{
		if (state == QUOTE)
		{
			error = std_string_format("no matching \" found for \" at %s:%d", source, lastQuote);
			return tokens;
		}
	}

	//¼ì²éÀ¨ºÅÆ¥Åä
	if (braceLayer == 1)
	{
		error = std_string_format("no matching closing bracket '}' for open bracket '{' at %s:%d",
			source, firstOpenBrace);
	}
	else if (braceLayer > 1)
	{
		error = std_string_format(
			"too many open brackets (%d) '{' without matching closing bracket '}' in %s", braceLayer,
			source);
	}

	return tokens;
}

void ScriptLexer::setToken(const std::string& lexeme, uint32_t line, const char* source, std::vector<ScriptToken>& tokens)
{
	//
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

	const auto len = lexeme.length();

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
	else if (len > 1 && lexeme[0] == var)
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
