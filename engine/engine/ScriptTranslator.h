#pragma once

class ScriptCompiler;
class AbstractNode;

#include <string>
#include <list>
#include "CMaterial.h"

class ScriptTranslator
{
public:
	virtual void translate(ScriptCompiler* compiler, AbstractNode* node) = 0;

protected:
	virtual ~ScriptTranslator() {}

public:
	static const AbstractNode* getNodeAt(const std::list<AbstractNode*>& nodes, int index);

	static void processNode(ScriptCompiler* compiler, AbstractNode* node);

	static bool getBoolean(const AbstractNode* node, bool& result);

	static bool getString(const AbstractNode* node, std::string& result);

	static bool getFloat(const AbstractNode* node, float& result);

	static bool getDouble(const AbstractNode* node, double& result);

	static bool getInt(const AbstractNode* node, int& result);

	static bool getUInt(const AbstractNode* node, uint32_t& result);

	static bool getRenderQueue(const AbstractNode* node, int& queue);

	static bool getLightMode(const AbstractNode* node, E_LIGHT_MODE& lightmode);
};

class MaterialTranslator : public ScriptTranslator
{
protected:
	std::map<std::string, CMaterial*>	m_MaterialMap;

public:
	MaterialTranslator() {}
	void translate(ScriptCompiler* compiler, AbstractNode* node) override;

	const std::map<std::string, CMaterial*>& getMaterialMap() const { return m_MaterialMap; }
};

class PassTranslator : public ScriptTranslator
{
public:
	PassTranslator() {}
	void translate(ScriptCompiler* compiler, AbstractNode* node) override;
};