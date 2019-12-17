#include "ScriptParser.h"
#include <cassert>

std::list<ConcreteNode*> ScriptParser::parse(const std::vector<ScriptToken>& tokens)
{
	std::list<ConcreteNode*> nodes;
	
	enum { READY, OBJECT };
	uint32_t state = READY;

	ConcreteNode* parent = nullptr;
	ConcreteNode* node;
	
	for (auto itr = tokens.begin(); itr != tokens.end();)
	{
		const ScriptToken& token = *itr;

		switch (state)
		{
		case READY:
		{
			if (token.type == TID_WORD)
			{
				if (token.lexeme == "import")
				{
					assert(false);
				}
				else if (token.lexeme == "set")
				{
					assert(false);
				}
				else
				{
					node = new ConcreteNode;
					node->file = token.file;
					node->line = token.line;
					if (token.type == TID_WORD)
					{
						node->type = CNT_WORD;
						node->token = token.lexeme;
					}
					else
					{
						node->type = CNT_QUOTE;
						node->token = token.lexeme.substr(1, token.lexeme.size() - 2);
					}

					if (parent)
					{
						node->parent = parent;
						parent->children.push_back(node);
					}
					else
					{
						node->parent = nullptr;
						nodes.push_back(node);
					}

					parent = node;
					node = nullptr;
					state = OBJECT;
				}
			}
			else if (token.type == TID_RBRACKET)
			{
				if (parent)
					parent = parent->parent;

				node = new ConcreteNode;
				node->token = token.lexeme;
				node->file = token.file;
				node->line = token.line;
				node->type = CNT_RBRACE;

				itr = skipNewLines(itr, tokens.end());

				if (parent)
				{
					node->parent = parent;
					parent->children.push_back(node);
				}
				else
				{
					node->parent = nullptr;
					nodes.push_back(node);
				}

				if (parent)
					parent = parent->parent;

				node = nullptr;
			}
		}
		break;
		case OBJECT:
		{
			if (token.type == TID_NEWLINE)
			{
				auto next = skipNewLines(itr, tokens.end());
				if (next == tokens.end() || itr->type != TID_LBRACKET)
				{
					if (parent)
						parent = parent->parent;
					state = READY;
				}
			} 
			else if (token.type == TID_COLON)
			{
			}
			else if (token.type == TID_LBRACKET)
			{
			}
			else if (token.type == TID_RBRACKET)
			{
			}
			else if (token.type == TID_VARIABLE)
			{
			}
			else if (token.type == TID_QUOTE)
			{
			}
			else if (token.type == TID_WORD)
			{
			}
		}
		break;
		default:
			break;
		}

		++itr;
	}

	return nodes;
}

std::list<ConcreteNode*> ScriptParser::parseChunk(const std::vector<ScriptToken>& tokens)
{
	std::list<ConcreteNode*> nodes;

	ConcreteNode* node = nullptr;
	for (const auto& token : tokens)
	{
		switch (token.type)
		{
		case TID_VARIABLE:
			node = new ConcreteNode;
			node->file = token.file;
			node->line = token.line;
			node->token = token.lexeme;
			node->type = CNT_VARIABLE;
			break;
		case TID_WORD:
			node = new ConcreteNode;
			node->file = token.file;
			node->line = token.line;
			node->token = token.lexeme;
			node->type = CNT_WORD;
			break;
		case TID_QUOTE:
			node = new ConcreteNode;
			node->file = token.file;
			node->line = token.line;
			node->token = token.lexeme.substr(1, token.lexeme.size()-2);
			node->type = CNT_QUOTE;
			break;
		default:
			assert(false);
			break;
		}

		if (node)
			nodes.push_back(node);
	}

	return nodes;
}

std::vector<ScriptToken>::const_iterator ScriptParser::skipNewLines(std::vector<ScriptToken>::const_iterator itr, std::vector<ScriptToken>::const_iterator end)
{
	while (itr != end && itr->type == TID_NEWLINE)
		++itr;
	return itr;
}
