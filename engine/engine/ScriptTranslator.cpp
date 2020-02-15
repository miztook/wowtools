#include "ScriptTranslator.h"
#include "ScriptCompiler.h"
#include "stringext.h"

const AbstractNode* ScriptTranslator::getNodeAt(const std::list<AbstractNode*>& nodes, int index)
{
	if (index >= (int)nodes.size())
		return nullptr;
	auto itr = std::next(nodes.begin(), index);
	return *itr;
}

void ScriptTranslator::processNode(ScriptCompiler* compiler, const AbstractNode* node)
{
	if (node->type != ANT_OBJECT)
		return;

	const ObjectAbstractNode* objNode = static_cast<const ObjectAbstractNode*>(node);

	ScriptTranslator* translator = compiler->getScriptCompilerManager()->getTranslator(node);
	if (translator)
		translator->translate(compiler, node);
	else
		compiler->addError(ScriptCompiler::CE_UNEXPECTEDTOKEN, node->file.c_str(), node->line,
			std_string_format("token \" %s \" is not recognized", objNode->cls.c_str()).c_str());
}

bool ScriptTranslator::getBoolean(const AbstractNode* node, bool& result)
{
	if (node->type != ANT_ATOM)
		return false;
	const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(node);
	if (atom->id == 1 || atom->id == 2)
	{
		result = atom->id == 1;
		return true;
	}
	return false;
}

bool ScriptTranslator::getString(const AbstractNode* node, std::string& result)
{
	if (node->type != ANT_ATOM)
		return false;
	const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(node);
	result = atom->value;
	return true;
}

bool ScriptTranslator::getFloat(const AbstractNode* node, float& result)
{
	if (node->type != ANT_ATOM)
		return false;
	const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(node);
	result = (float)atof(atom->value.c_str());
	return true;
}

bool ScriptTranslator::getDouble(const AbstractNode* node, double& result)
{
	if (node->type != ANT_ATOM)
		return false;
	const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(node);
	result = atof(atom->value.c_str());
	return true;
}

bool ScriptTranslator::getInt(const AbstractNode* node, int& result)
{
	if (node->type != ANT_ATOM)
		return false;
	const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(node);
	result = atoi(atom->value.c_str());
	return true;
}

bool ScriptTranslator::getUInt(const AbstractNode* node, uint32_t& result)
{
	if (node->type != ANT_ATOM)
		return false;
	const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(node);
	result = (uint32_t)atoi(atom->value.c_str());
	return true;
}

template <typename T>
bool getValue(const AbstractNode* node, T& result);
template<> bool getValue(const AbstractNode* node, float& result)
{
	return ScriptTranslator::getFloat(node, result);
}
template<> bool getValue(const AbstractNode* node, double& result)
{
	return ScriptTranslator::getDouble(node, result);
}
template<> bool getValue(const AbstractNode* node, bool& result)
{
	return ScriptTranslator::getBoolean(node, result);
}
template<> bool getValue(const AbstractNode* node, uint32_t& result)
{
	return ScriptTranslator::getUInt(node, result);
}
template<> bool getValue(const AbstractNode* node, int& result)
{
	return ScriptTranslator::getInt(node, result);
}
template<> bool getValue(const AbstractNode* node, std::string& result)
{
	return ScriptTranslator::getString(node, result);
}

template <typename T>
static bool getValue(const PropertyAbstractNode* prop, ScriptCompiler *compiler, T& val)
{
	if (prop->values.empty())
	{
		compiler->addError(ScriptCompiler::CE_STRINGEXPECTED, prop->file.c_str(), prop->line);
	}
	else if (prop->values.size() > 1)
	{
		compiler->addError(ScriptCompiler::CE_FEWERPARAMETERSEXPECTED, prop->file.c_str(), prop->line,
			(getPropertyName(compiler, prop->id) + " must have at most 1 argument").c_str());
	}
	else
	{
		if (getValue(prop->values.front(), val))
			return true;
		else
			compiler->addError(ScriptCompiler::CE_INVALIDPARAMETERS, prop->file.c_str(), prop->line,
				std_string_format("%s is not a valid value for %s", prop->values.front()->getValue(), getPropertyName(compiler, prop->id).c_str()).c_str());
	}

	return false;
}

void MaterialTranslator::translate(ScriptCompiler* compiler, const AbstractNode* node)
{
	const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);
	if (obj->name.empty())
		compiler->addError(ScriptCompiler::CE_OBJECTNAMEEXPECTED, obj->file.c_str(), obj->line);

	bool bval;
	for (auto itr = obj->children.begin(); itr != obj->children.end(); ++itr)
	{

	}
}

void PassTranslator::translate(ScriptCompiler* compiler, const AbstractNode* node)
{
	const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);

	for (auto itr = obj->children.begin(); itr != obj->children.end(); ++itr)
	{

	}
}