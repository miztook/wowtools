#include "ScriptParser.h"
#include <cassert>

std::list<ConcreteNode*> ScriptParser::parse(const std::vector<ScriptToken>& tokens)
{
	std::list<ConcreteNode*> nodes;
	
	enum { READY, OBJECT };
	uint32_t state = READY;

	ConcreteNode* parent = nullptr;
	ConcreteNode* node;
	auto end = tokens.end();
	for (auto itr = tokens.begin(); itr != tokens.end();)
	{
		const ScriptToken& token = *itr;

		switch (state)
		{
		case READY:
		{
			if (token.type == TID_WORD || token.type == TID_QUOTE)
			{
				if (token.lexeme == "import" || token.lexeme == "set")
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

				itr = skipNewLines(itr, end);

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
				auto next = skipNewLines(itr, end);
				if (next == end || next->type != TID_LBRACKET)
				{
					if (parent)
						parent = parent->parent;
					state = READY;
				}
			} 
			else if (token.type == TID_COLON)
			{
				node = new ConcreteNode;
				node->token = token.lexeme;
				node->file = token.file;
				node->line = token.line;
				node->type = CNT_COLON;

				auto j = itr + 1;
				j = skipNewLines(j, end);
				if (j == end || (j->type != TID_WORD && j->type != TID_QUOTE))
				{
					assert(false);
				}

				while (j != end && (j->type == TID_WORD || j->type == TID_QUOTE))
				{
					ConcreteNode* tempNode = new ConcreteNode;
					if (j->type == TID_WORD)
						tempNode->token = j->lexeme;
					else
						tempNode->token = j->lexeme.substr(1, j->lexeme.length() - 2);
					tempNode->file = j->file;
					tempNode->line = j->line;
					tempNode->type = j->type == TID_WORD ? CNT_WORD : CNT_QUOTE;
					tempNode->parent = node;
					node->children.push_back(tempNode);
					++j;
				}
				itr = --j;

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

				node = nullptr;
			}
			else if (token.type == TID_LBRACKET)
			{
				node = new ConcreteNode;
				node->token = token.lexeme;
				node->file = token.file;
				node->line = token.line;
				node->type = CNT_LBRACE;

				itr = skipNewLines(itr, end);

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

				state = READY;

				node = nullptr;
			}
			else if (token.type == TID_RBRACKET)
			{
				if (parent)
					parent = parent->parent;

				if (parent && parent->type == CNT_LBRACE && parent->parent)
					parent = parent->parent;

				node = new ConcreteNode;
				node->token = token.lexeme;
				node->file = token.file;
				node->line = token.line;
				node->type = CNT_RBRACE;

				itr = skipNewLines(itr, end);

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

				state = READY;

				node = nullptr;
			}
			else if (token.type == TID_VARIABLE)
			{
				node = new ConcreteNode;
				node->token = token.lexeme;
				node->file = token.file;
				node->line = token.line;
				node->type = CNT_VARIABLE;

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

				node = nullptr;
			}
			else if (token.type == TID_QUOTE)
			{
				node = new ConcreteNode;
				node->token = token.lexeme.substr(1, token.lexeme.length() - 2);
				node->file = token.file;
				node->line = token.line;
				node->type = CNT_QUOTE;

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

				node = nullptr;
			}
			else if (token.type == TID_WORD)
			{
				node = new ConcreteNode;
				node->token = token.lexeme;
				node->file = token.file;
				node->line = token.line;
				node->type = CNT_WORD;

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

				node = nullptr;
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
