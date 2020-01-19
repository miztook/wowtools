#pragma once

class ScriptCompiler;
class AbstractNode;

#include <string>
#include <list>

class ScriptTranslator
{
public:
	virtual void translate(ScriptCompiler* compiler, const AbstractNode* node) = 0;

protected:
	virtual ~ScriptTranslator() {}

	static const AbstractNode* getNodeAt(const std::list<AbstractNode*>& nodes, int index);

	static void processNode(ScriptCompiler* compiler, const AbstractNode* node);

	static bool getBoolean(const AbstractNode* node, bool& result);

	static bool getString(const AbstractNode* node, std::string& result);

	static bool getFloat(const AbstractNode* node, float& result);

	static bool getDouble(const AbstractNode* node, double& result);

	static bool getInt(const AbstractNode* node, int& result);

	static bool getUInt(const AbstractNode* node, uint32_t& result);

	template<typename T>
	friend bool getValue(const AbstractNode* node, T& result);
};

class MaterialTranslator : public ScriptTranslator
{
protected:

public:
	MaterialTranslator() {}
	void translate(ScriptCompiler* compiler, const AbstractNode* node) override;
};

class TechniqueTranslator : public ScriptTranslator
{
protected:

public:
	TechniqueTranslator() {}
	void translate(ScriptCompiler* compiler, const AbstractNode* node) override;
};

class PassTranslator : public ScriptTranslator
{
protected:

public:
	PassTranslator() {}
	void translate(ScriptCompiler* compiler, const AbstractNode* node) override;
};

class TextureUnitTranslator : public ScriptTranslator
{
protected:

public:
	TextureUnitTranslator() {}
	void translate(ScriptCompiler* compiler, const AbstractNode* node) override;
};

class SamplerTranslator : public ScriptTranslator
{
protected:

public:
	SamplerTranslator() {}
	void translate(ScriptCompiler* compiler, const AbstractNode* node) override;
};

class TextureSourceTranslator : public ScriptTranslator
{
protected:

public:
	TextureSourceTranslator() {}
	void translate(ScriptCompiler* compiler, const AbstractNode* node) override;
};

class GpuProgramTranslator : public ScriptTranslator
{
protected:

public:
	GpuProgramTranslator() {}
	void translate(ScriptCompiler* compiler, const AbstractNode* node) override;
};

class SharedParamsTranslator : public ScriptTranslator
{
protected:

public:
	SharedParamsTranslator() {}
	void translate(ScriptCompiler* compiler, const AbstractNode* node) override;
};