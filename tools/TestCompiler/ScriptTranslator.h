#pragma once

class ScriptCompiler;
class AbstractNode;

#include <string>

class ScriptTranslator
{
public:
	virtual void translate(ScriptCompiler* compiler, const AbstractNode* node) = 0;

protected:
	virtual ~ScriptTranslator() {}

	static void processNode(ScriptCompiler* compiler, const AbstractNode* node);

	static bool getBoolean(const AbstractNode* node, bool& result);

	static bool getString(const AbstractNode* node, std::string& result);

	static bool getFloat(const AbstractNode* node, float& result);

	static bool getInt(const AbstractNode* node, int& result);

	static bool getUInt(const AbstractNode* node, uint32_t& result);
};