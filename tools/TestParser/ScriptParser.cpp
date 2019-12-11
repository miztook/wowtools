#include "ScriptParser.h"


std::vector<ScriptToken>::iterator ScriptParser::skipNewLines(std::vector<ScriptToken>::iterator itr, std::vector<ScriptToken>::iterator end)
{
	while (itr != end && itr->type == TID_NEWLINE)
		++itr;
	return itr;
}
