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

	for (auto itr = astList->begin(); itr != astList->end(); ++itr)
	{
		ScriptTranslator* translator = m_scriptCompilerManager->getTranslator(*itr);
		if (translator)
			translator->translate(this, *itr);
	}

	return m_Errors.empty();
}

std::list<AbstractNode*>* ScriptCompiler::_generateAST(const char* str, const char* source, bool doVariables /*= false*/)
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

	if (astNodes && doVariables)
		processVariables(*astNodes);

	return astNodes;
}

void ScriptCompiler::processImports(const std::list<AbstractNode*>& nodes)
{

}

void ScriptCompiler::processObjects(std::list<AbstractNode*>& nodes, const std::list<AbstractNode*>& top)
{

}
