#include "ScriptCompiler.h"
#include "ScriptParser.h"
#include "ScriptTranslator.h"
#include "stringext.h"

struct IDEntry
{
	const char* str;
	int id;
};

IDEntry g_IdEntries[] =
{
	{ "On", ID_ON },
	{ "Off", ID_OFF },
	{ "True", ID_TRUE },
	{ "False", ID_FALSE },
	{ "Yes", ID_YES },
	{ "No", ID_NO },

	{ "Material", ID_MATERIAL },
	{ "Pass", ID_PASS },

	{ "Queue", ID_QUEUE },
	{ "Background", ID_BACKGROUND },
	{ "Geometry", ID_GEOMETRY },
	{ "AlphaTest", ID_ALPHATEST },
	{ "GeometryLast", ID_GEOMETRYLAST },
	{ "Overlay", ID_OVERLAY },

	{ "LightMode", ID_LIGHT_MODE },
	{ "Always", ID_ALWAYS },
	{ "ForwardBase", ID_FORWARD_BASE },
	{ "ForwardAdd", ID_FORWARD_ADD },

	{ "Cull", ID_CULL },
	{ "Back", ID_BACK },
	{ "Front", ID_FRONT },
	{ "None", ID_NONE },

	{ "AntiAliasing", ID_ANTIALIASING },
	{ "Simple", ID_SIMPLE },
	{ "LineSmooth", ID_LINE_SMOOTH },
	
	{ "ZTest", ID_ZTEST },
	{ "Never", ID_NEVER },
	{ "LessEqual", ID_LESSEQUAL },
	{ "Equal", ID_EQUAL },
	{ "Less", ID_LESS },
	{ "NotEqual", ID_NOTEQUAL },
	{ "GreaterEqual", ID_GREATEREQUAL },
	{ "Greater", ID_GREATER },

	{ "ZWrite", ID_ZWRITE },
	
	{ "ColorMask", ID_COLOR_MASK },
	{ "R", ID_R },
	{ "RG", ID_RG },
	{ "RGB", ID_RGB },
	{ "G", ID_G },
	{ "GB", ID_GB },
	{ "B", ID_B },

	{ "AlphaBlend", ID_ALPHABLEND },
	{ "Zero", ID_ZERO },
	{ "One", ID_ONE },
	{ "DstColor", ID_DST_COLOR },
	{ "OneMinusDstColor", ID_ONE_MINUS_DST_COLOR },
	{ "SrcColor", ID_SRC_COLOR },
	{ "OneMinusSrcColor", ID_ONE_MINUS_SRC_COLOR },
	{ "SrcAlpha", ID_SRC_ALPHA },
	{ "OneMinusSrcAlpha", ID_ONE_MINUS_SRC_ALPHA },
	{ "DstAlpha", ID_DST_ALPHA },
	{ "OneMinusDstAlpha", ID_ONE_MINUS_DST_ALPHA },

	{ "Define", ID_DEFINE },
	{ "VSFile", ID_VSFILE },
	{ "PSFile", ID_PSFILE },
};

const char* ScriptCompiler::formatErrorCode(uint32_t code)
{
	switch (code)
	{
	case CE_STRINGEXPECTED:
		return "string expected";
	case CE_NUMBEREXPECTED:
		return "number expected";
	case CE_FEWERPARAMETERSEXPECTED:
		return "fewer parameters expected";
	case CE_VARIABLEEXPECTED:
		return "variable expected";
	case CE_UNDEFINEDVARIABLE:
		return "undefined variable";
	case CE_OBJECTNAMEEXPECTED:
		return "object name expected";
	case CE_OBJECTALLOCATIONERROR:
		return "object allocation error";
	case CE_OBJECTBASENOTFOUND:
		return "base object not found";
	case CE_INVALIDPARAMETERS:
		return "invalid parameters";
	case CE_DUPLICATEOVERRIDE:
		return "duplicate object override";
	case CE_REFERENCETOANONEXISTINGOBJECT:
		return "reference to a non existing object";
	case CE_UNEXPECTEDTOKEN:
		return "unexpected token";
	case CE_DEPRECATEDSYMBOL:
		return "deprecated symbol";
	default:
		return "unknown error";
	}
}

ScriptCompiler::ScriptCompiler(MaterialCompiler* scriptCompilerManager)
	: m_scriptCompilerManager(scriptCompilerManager)
{
	initWordMap();
}

bool ScriptCompiler::compile(const char* str, const char* source)
{
	std::string error;
	std::vector<ScriptToken> tokens = ScriptLexer::tokenize(str, source, error);

	if (!error.empty())
	{
		ASSERT(false);
		return false;
	}

	std::list<ConcreteNode*> nodes = ScriptParser::parse(tokens);
	return compile(nodes);
}

bool ScriptCompiler::compile(const std::list<ConcreteNode*>& nodes)
{
	m_Errors.clear();

	std::list<AbstractNode*> astList = convertToAST(nodes);


	for (AbstractNode* node : astList)
	{
		ScriptTranslator* translator = m_scriptCompilerManager->getTranslator(node);
		if (translator)
			translator->translate(this, node);
	}

	for (AbstractNode* node : astList)
	{
		AbstractNode::deleteNode(node);
	}
	astList.clear();

	return m_Errors.empty();
}

void ScriptCompiler::addError(uint32_t code, const char* file, int line, const char* msg /*= ""*/)
{
	ASSERT(false);

	//Êä³ö´íÎó
	std::string errMsg = std_string_format("ScriptCompiler - %s in %s (%d)",
		ScriptCompiler::formatErrorCode(code), file, line);
	if (strlen(msg) > 0)
	{
		errMsg += ": ";
		errMsg += msg;
	}
	printf("%s\n", errMsg.c_str());

	ScriptCompiler::Error err;
	err.file = file;
	err.message = msg;
	err.line = line;
	err.code = code;
	m_Errors.push_back(err);
}

std::list<AbstractNode*> ScriptCompiler::convertToAST(const std::list<ConcreteNode*>& nodes)
{
	AbstractTreeBuilder builder(this);
	AbstractTreeBuilder::visit(&builder, nodes);
	return builder.getResult();
}

void ScriptCompiler::initWordMap()
{
	for (int i = 0; i < ARRAY_COUNT(g_IdEntries); ++i)
	{
		m_Ids[g_IdEntries[i].str] = g_IdEntries[i].id;
	}
}

std::string getPropertyName(const ScriptCompiler* compiler, uint32_t id)
{
	for (const auto& kv : compiler->m_Ids)
	{
		if (kv.second == id)
			return kv.first;
	}
	ASSERT(false);
	return "unknown";
}

ScriptCompiler::AbstractTreeBuilder::AbstractTreeBuilder(ScriptCompiler* compiler)
	: mCompiler(compiler)
{
	mCurrent = nullptr;
}

void ScriptCompiler::AbstractTreeBuilder::visit(AbstractTreeBuilder* visitor, const std::list<ConcreteNode*>& nodes)
{
	for (auto node : nodes)
		visitor->visit(node);
}

void ScriptCompiler::AbstractTreeBuilder::visit(ConcreteNode* node)
{
	AbstractNode* asn = nullptr;

	if (node->type == CNT_IMPORT && !mCurrent)
	{
		ASSERT(false);
	}
	else if (node->type == CNT_VARIABLE_ASSIGN)	  // variable set = "set" >> 2 children, children[0] == variable
	{
		ASSERT(false);
	}
	else if (node->type == CNT_VARIABLE)
	{
		ASSERT(false);
	}
	else if (!node->children.empty())			//properties and objects
	{
		ConcreteNode* temp1 = nullptr;
		ConcreteNode* temp2 = nullptr;
		auto riter = node->children.rbegin();

		if (riter != node->children.rend())
		{
			temp1 = *riter;
			++riter;
		}
		if (riter != node->children.rend())
		{
			temp2 = *riter;
		}

		// object = last 2 children == { and }
		if (temp1 && temp2 && temp1->type == CNT_RBRACE && temp2->type == CNT_LBRACE)
		{
			if (node->children.size() < 2)
			{
				mCompiler->addError(CE_STRINGEXPECTED, node->file.c_str(), node->line);
				return;
			}

			ObjectAbstractNode* impl = new ObjectAbstractNode(mCurrent);
			impl->line = node->line;
			impl->file = node->file;

			std::list<ConcreteNode*> temp;
			if (node->token == "abstract")
			{
				ASSERT(false);
				//impl->abstract = true;
				for (ConcreteNode* n : node->children)
				{
					temp.push_back(n);
				}
			}
			else
			{
				temp.push_back(node);
				for (ConcreteNode* n : node->children)
				{
					temp.push_back(n);
				}
			}

			//Get the type of object
			auto iter = temp.begin();
			impl->cls = (*iter)->token;
			++iter;

			//try to map the cls to an id
			auto iter2 = mCompiler->m_Ids.find(impl->cls);
			if (iter2 != mCompiler->m_Ids.end())
				impl->id = iter2->second;
			else
				mCompiler->addError(CE_UNEXPECTEDTOKEN, impl->file.c_str(), impl->line,
					std_string_format("'%s'. If this is a legacy script you must prepend the type (e.g. font, overlay).", impl->cls.c_str()).c_str());

			// Get the name
			// Unless the type is in the exclusion list
			if (iter != temp.end() && ((*iter)->type == CNT_WORD || (*iter)->type == CNT_QUOTE))
			{
				impl->name = (*iter)->token;
				++iter;
			}

			// Everything up until the colon is a "value" of this object
			while (iter != temp.end() && (*iter)->type != CNT_COLON && (*iter)->type != CNT_LBRACE)
			{
				if ((*iter)->type == CNT_VARIABLE)
				{
					ASSERT(false);
				}
				else
				{
					AtomAbstractNode* atom = new AtomAbstractNode(impl);
					atom->file = (*iter)->file;
					atom->line = (*iter)->line;
					atom->type = ANT_ATOM;
					atom->value = (*iter)->token;

					auto idpos = mCompiler->m_Ids.find(atom->value);
					if (idpos != mCompiler->m_Ids.end())
						atom->id = idpos->second;

					impl->values.push_back(atom);
				}
				++iter;
			}

			//Find the bases
			if (iter != temp.end() && (*iter)->type == CNT_COLON)
			{

				ASSERT(false);
				// 				for (ConcreteNode* n : (*iter)->children)
				// 				{
				// 					impl->bases.push_back(n->token);
				// 				}
				// 				++iter;
			}

			asn = impl;
			mCurrent = impl;

			// Visit the children of the {
			AbstractTreeBuilder::visit(this, temp2->children);

			mCurrent = impl->parent;
		}
		else  //it's a property
		{
			PropertyAbstractNode* impl = new PropertyAbstractNode(mCurrent);
			impl->line = node->line;
			impl->file = node->file;
			impl->name = node->token;

			auto iter2 = mCompiler->m_Ids.find(impl->name);
			if (iter2 != mCompiler->m_Ids.end())
				impl->id = iter2->second;

			asn = impl;
			mCurrent = impl;

			// Visit the children of the {
			AbstractTreeBuilder::visit(this, node->children);

			mCurrent = impl->parent;
		}
	}
	else            // standard atom
	{
		AtomAbstractNode* impl = new AtomAbstractNode(mCurrent);
		impl->line = node->line;
		impl->file = node->file;
		impl->value = node->token;

		auto iter2 = mCompiler->m_Ids.find(impl->value);
		if (iter2 != mCompiler->m_Ids.end())
			impl->id = iter2->second;

		asn = impl;
	}

	if (asn)
	{
		if (mCurrent)
		{
			if (mCurrent->type == ANT_PROPERTY)
			{
				PropertyAbstractNode* impl = static_cast<PropertyAbstractNode*>(mCurrent);
				impl->values.push_back(asn);
			}
			else
			{
				ObjectAbstractNode* impl = static_cast<ObjectAbstractNode*>(mCurrent);
				impl->children.push_back(asn);
			}
		}
		else
		{
			mNodes.push_back(asn);
		}
	}
}

MaterialCompiler::MaterialCompiler()
	: m_ScriptCompiler(this)
{

}

MaterialCompiler::~MaterialCompiler()
{

}

ScriptTranslator* MaterialCompiler::getTranslator(const AbstractNode* node)
{
	ScriptTranslator* translator = nullptr;
	if (node->type == ANT_OBJECT)
	{
		const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);
		const ObjectAbstractNode* parent = obj->parent ? static_cast<const ObjectAbstractNode*>(obj->parent) : nullptr;

		if (obj->id == ID_MATERIAL)
		{
			translator = &m_MaterialTranslator;
		}
		else if (obj->id == ID_PASS && parent->id == ID_MATERIAL)
		{
			translator = &m_PassTranslator;
		}
		else
		{
			ASSERT(false);
		}
	}
	return translator;
}

bool MaterialCompiler::parseScript(const char* str, const char* source)
{
	std::string error;
	std::vector<ScriptToken> tokenList = ScriptLexer::tokenize(str, source, error);
	if (!error.empty())
		return false;

	std::list<ConcreteNode*> nodes = ScriptParser::parse(tokenList);
	bool ret = m_ScriptCompiler.compile(nodes);
	for (ConcreteNode* n : nodes)
		ConcreteNode::deleteNode(n);
	return ret;
}

std::list<AbstractNode*> MaterialCompiler::convertToAST(const std::list<ConcreteNode*>& nodes)
{
	return m_ScriptCompiler.convertToAST(nodes);
}

const std::map<std::string, CMaterial*>& MaterialCompiler::getMaterialMap() const
{
	return m_MaterialTranslator.getMaterialMap();
}
