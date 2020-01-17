#include "ScriptCompiler.h"
#include "base.h"
#include "ScriptParser.h"
#include "ScriptTranslator.h"

AbstractNode::AbstractNode(AbstractNode* _parent)
	: line(0), type(ANT_UNKNOWN), parent(_parent) 
{
}

AtomAbstractNode::AtomAbstractNode(AbstractNode* _parent)
	: AbstractNode(_parent), id (0)
{
	type = ANT_ATOM;
}

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

ScriptCompiler::ScriptCompiler(ScriptCompilerManager* scriptCompilerManager)
	: m_Listener(nullptr), m_scriptCompilerManager(scriptCompilerManager)
{

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
	m_Env.clear();

	if (m_Listener)
		m_Listener->preConversion(this, nodes);

	std::list<AbstractNode*>* astList = convertToAST(nodes);
	ASSERT(astList);

	processVariables(*astList);

	if (m_Listener && !m_Listener->postConversion(this, *astList))
		return m_Errors.empty();

	for (const AbstractNode* node : *astList)
	{
		ScriptTranslator* translator = m_scriptCompilerManager->getTranslator(node);
		if (translator)
			translator->translate(this, node);
	}

	return m_Errors.empty();
}

std::list<AbstractNode*>* ScriptCompiler::_generateAST(const char* str, const char* source, bool doObjects, bool doVariables /*= false*/)
{
	std::string error;
	std::vector<ScriptToken> tokens = ScriptLexer::tokenize(str, source, error);

	if (!error.empty())
	{
		ASSERT(false);
		return nullptr;
	}

	std::list<ConcreteNode*> nodes = ScriptParser::parse(tokens);

	if (m_Listener)
		m_Listener->preConversion(this, nodes);

	std::list<AbstractNode*>* astNodes = convertToAST(nodes);

	if (astNodes && doObjects)
		processObjects(*astNodes, *astNodes);
	
	if (astNodes && doVariables)
		processVariables(*astNodes);

	return astNodes;
}

bool ScriptCompiler::_compile(std::list<AbstractNode*>& nodes, bool doObjects, bool doVariables /*= false*/)
{
	m_Errors.clear();

	m_Env.clear();

	if (doObjects)
		processObjects(nodes, nodes);

	if (doVariables)
		processVariables(nodes);

	for (const AbstractNode* node : nodes)
	{
		if (node->type == ANT_OBJECT && static_cast<const ObjectAbstractNode*>(node)->abstract)
			continue;

		ScriptTranslator* translator = m_scriptCompilerManager->getTranslator(node);
		if (translator)
			translator->translate(this, node);
	}

	return m_Errors.empty();
}

void ScriptCompiler::addError(uint32_t code, const char* file, int line, const char* msg /*= ""*/)
{
	if (m_Listener)
	{
		m_Listener->handleError(this, code, file, line, msg);
	}
	else
	{
		ASSERT_TODO
	}

	ScriptCompiler::Error err;
	err.file = file;
	err.message = msg;
	err.line = line;
	err.code = code;
	m_Errors.push_back(err);
}

bool ScriptCompiler::_fireEvent(ScriptCompilerEvent* evt, void* retVal)
{
	if (m_Listener)
		return m_Listener->handleEvent(this, evt, retVal);
	return false;
}

std::list<AbstractNode*>* ScriptCompiler::convertToAST(const std::list<ConcreteNode*>& nodes)
{
	AbstractTreeBuilder builder(this);
	AbstractTreeBuilder::visit(&builder, nodes);
	return builder.getResult();
}

void ScriptCompiler::processImports(std::list<AbstractNode*>& nodes)
{
	ASSERT_TODO
}

std::list<AbstractNode*>* ScriptCompiler::loadImportPath(const char* name)
{
	ASSERT_TODO
	return nullptr;
}

std::list<AbstractNode*> ScriptCompiler::locateTarget(const std::list<AbstractNode*>& nodes, const char* target)
{
	ASSERT_TODO
	return std::list<AbstractNode*>();
}

void ScriptCompiler::processObjects(std::list<AbstractNode*>& nodes, const std::list<AbstractNode*>& top)
{
	for (auto itr = nodes.begin(); itr != nodes.end(); ++itr)
	{
		if ((*itr)->type != ANT_OBJECT)
			continue;

		ObjectAbstractNode* obj = static_cast<ObjectAbstractNode*>(*itr);

		for (const auto& base : obj->bases)
		{
			std::list<AbstractNode*> newNodes = locateTarget(top, base.c_str());
			if (newNodes.empty())
				addError(CE_OBJECTBASENOTFOUND, obj->file.c_str(), obj->line, base.c_str());

			for (const auto& n : newNodes)
			{
				if (n->type != ANT_OBJECT)
					continue;
				ASSERT_TODO
			}
		}

		processObjects(obj->children, top);

		obj->children.insert(obj->children.begin(), obj->overrides.begin(), obj->overrides.end());
	}
}

void ScriptCompiler::processVariables(std::list<AbstractNode*>& nodes)
{
	auto i = nodes.begin();
	while (i != nodes.end())
	{
		auto cur = i;
		AbstractNode* node = *cur;
		++i;

		if (node->type == ANT_OBJECT)
		{
			ASSERT_TODO
		}
		else if (node->type == ANT_PROPERTY)
		{
			PropertyAbstractNode* prop = static_cast<PropertyAbstractNode*>(node);
			processVariables(prop->values);
		}
		else if (node->type == ANT_VARIABLE_ACCESS)
		{
			VariableAccessAbstractNode* var = static_cast<VariableAccessAbstractNode*>(node);

			ObjectAbstractNode* scope = nullptr;
			AbstractNode* temp = var->parent;
			while (temp)
			{
				if (temp->type == ANT_OBJECT)
				{
					ASSERT(false);
					scope = static_cast<ObjectAbstractNode*>(temp);
					break;
				}
				temp = temp->parent;
			}

			std::pair<bool, std::string> varAccess;
			if (scope)
				varAccess = scope->getVariable(var->name.c_str());

			if (!scope || !varAccess.first)
			{
				auto k = m_Env.find(var->name.c_str());
				varAccess.first = k != m_Env.end();
				if (varAccess.first)
					varAccess.second = k->second;
			}

			if (varAccess.first)
			{
				std::string err;
				std::vector<ScriptToken> tokens = ScriptLexer::tokenize(varAccess.second.c_str(), var->file.c_str(), err);
				ASSERT(err.empty());
				std::list<ConcreteNode*> cst = ScriptParser::parseChunk(tokens);
				std::list<AbstractNode*>* ast = convertToAST(cst);

				for (AbstractNode* node : *ast)
					node->parent = var->parent;
				
				processVariables(*ast);

				nodes.insert(cur, ast->begin(), ast->end());
			}
			else
			{
				addError(CE_UNDEFINEDVARIABLE, var->file.c_str(), var->line, var->name.c_str());
			}

			nodes.erase(cur);
		}
	}
}

ScriptCompiler::AbstractTreeBuilder::AbstractTreeBuilder(ScriptCompiler* compiler)
	: mCompiler(compiler)
{
	mNodes = new std::list<AbstractNode*>();
	mCurrent = nullptr;
}

void ScriptCompiler::AbstractTreeBuilder::visit(AbstractTreeBuilder* visitor, const std::list<ConcreteNode*>& nodes)
{
	for (auto node : nodes)
		visitor->visit(node);
}

ScriptTranslator* ScriptCompilerManager::getTranslator(const AbstractNode* node)
{
	ScriptTranslator* translator = nullptr;
	if (node->type == ANT_OBJECT)
	{
		const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);
		const ObjectAbstractNode* parent = obj->parent ? static_cast<const ObjectAbstractNode*>(obj->parent) : nullptr;

		ASSERT_TODO
	}
	return translator;
}

ObjectAbstractNode::ObjectAbstractNode(AbstractNode* _parent)
	: AbstractNode(_parent), id (0), abstract(false)
{
	type = ANT_OBJECT;
}

void ObjectAbstractNode::addVariable(const char* name)
{
	mEnv[name] = "";
}

void ObjectAbstractNode::setVariable(const char* name, const char* value)
{
	mEnv[name] = value;
}

std::pair<bool, std::string> ObjectAbstractNode::getVariable(const char* name) const
{
	auto itr = mEnv.find(name);
	if (itr != mEnv.end())
		return std::make_pair(true, itr->second);

	ObjectAbstractNode* parentNode = static_cast<ObjectAbstractNode*>(parent);
	while (parentNode)
	{
		itr = parentNode->mEnv.find(name);
		if (itr != parentNode->mEnv.end())
			return std::make_pair(true, itr->second);
		parentNode = static_cast<ObjectAbstractNode*>(parentNode->parent);
	}
	return std::make_pair(false, "");
}

PropertyAbstractNode::PropertyAbstractNode(AbstractNode* _parent)
	: AbstractNode(_parent), id (0)
{
	type = ANT_PROPERTY;
}

ImportAbstractNode::ImportAbstractNode()
	: AbstractNode(nullptr)
{
	type = ANT_IMPORT;
}

VariableAccessAbstractNode::VariableAccessAbstractNode(AbstractNode* _parent)
	: AbstractNode(_parent)
{
	type = ANT_VARIABLE_ACCESS;
}
