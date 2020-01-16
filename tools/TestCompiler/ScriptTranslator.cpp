#include "ScriptTranslator.h"
#include "ScriptCompiler.h"
#include "stringext.h"

void ScriptTranslator::processNode(ScriptCompiler* compiler, const AbstractNode* node)
{
	if (node->type != ANT_OBJECT)
		return;

	const ObjectAbstractNode* objNode = static_cast<const ObjectAbstractNode*>(node);
	if (objNode->abstract)
		return;

	ScriptTranslator* translator = compiler->getScriptCompilerManager()->getTranslator(node);
	if (translator)
		translator->translate(compiler, node);
	else
		compiler->addError(ScriptCompiler::CE_UNEXPECTEDTOKEN, node->file.c_str(), node->line,
			std_string_format("token \" %s \" is not recognized", objNode->cls.c_str()).c_str());
}
