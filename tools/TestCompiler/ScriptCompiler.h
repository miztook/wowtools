#pragma once

#include <vector>
#include <list>
#include <map>
#include <stdbool.h>

#include "varianttype.h"

class ConcreteNode;

using VAR_T = Variant<uint32_t, uint64_t, uint16_t, int, float, std::string>;

enum AbstractNodeType : int
{
	ANT_UNKNOWN,
	ANT_ATOM,
	ANT_OBJECT,
	ANT_PROPERTY,
	ANT_IMPORT,
	ANT_VARIABLE_SET,
	ANT_VARIABLE_ACCESS,
};

class AbstractNode 
{
public:
	std::string file;
	uint32_t line;
	AbstractNodeType type;
	AbstractNode* parent;
	VAR_T	context;

public:
	explicit AbstractNode(AbstractNode* _parent);
	virtual ~AbstractNode() {}

	virtual const char* getValue() const = 0;
};

class AtomAbstractNode : public AbstractNode
{
public:
	std::string value;
	uint32_t id;
public:
	explicit AtomAbstractNode(AbstractNode* _parent);
	const char* getValue() const override { return value.c_str(); }
};

class ObjectAbstractNode : public AbstractNode
{
private:
	std::map<std::string, std::string> mEnv;
public:
	std::string name;
	std::string cls;
	std::vector<string> bases;
	uint32_t id;
	bool abstract;
	std::list<AbstractNode*>	children;
	std::list<AbstractNode*>	values;
	std::list<AbstractNode*>	overrides;

public:
	ObjectAbstractNode(AbstractNode* _parent);
	const char* getValue() const override { return cls.c_str(); }

	void addVariable(const char* name);
	void setVariable(const char* name, const char* value);
	std::pair<bool, std::string> getVariable(const char* name) const;
	const std::map<std::string, std::string>& getVariables() const;

};

class PropertyAbstractNode : public AbstractNode
{
public: 
	std::string name;
	uint32_t id;
	std::list<AbstractNode*>	values;
public:
	PropertyAbstractNode(AbstractNode* _parent);
	const char* getValue() const override { return name.c_str(); }
};

class ImportAbstractNode : public AbstractNode
{
public:
	std::string target;
	std::string source;
public:
	ImportAbstractNode();
	const char* getValue() const override { return target.c_str(); }
};

class ScriptCompilerListener;
class ScriptCompilerEvent;

class ScriptCompiler
{
public:
	enum 
	{
		CE_STRINGEXPECTED,
		CE_NUMBEREXPECTED,
		CE_FEWERPARAMETERSEXPECTED,
		CE_VARIABLEEXPECTED,
		CE_UNDEFINEDVARIABLE,
		CE_OBJECTNAMEEXPECTED,
		CE_OBJECTALLOCATIONERROR,
		CE_INVALIDPARAMETERS,
		CE_DUPLICATEOVERRIDE,
		CE_UNEXPECTEDTOKEN,
		CE_OBJECTBASENOTFOUND,
		CE_REFERENCETOANONEXISTINGOBJECT,
		CE_DEPRECATEDSYMBOL,
	};

	static const char* formatErrorCode(uint32_t code);

public:
	ScriptCompiler();
	virtual ~ScriptCompiler() {}

	bool compile(const char* str, const char* source);
	bool compile(const std::list<ConcreteNode*>& nodes);
	std::list<AbstractNode*>* _generateAST(const char* str, const char* source, bool doObjects = false, bool doVariables = false);

	bool _compile(const std::list<ConcreteNode*>& nodes, bool doObjects = false, bool doVariables = false);
	void addError(uint32_t code, const char* file, int line, const char* msg = "");
	void setListener(ScriptCompilerListener* listener);
	ScriptCompilerListener* getListener() const;
	bool _fireEvent(ScriptCompilerEvent* evt, void* retVal);

private:
	std::list<AbstractNode*>* convertToAST(const std::list<ConcreteNode*>& nodes);

	void processImports(const std::list<AbstractNode*>& nodes);

	std::list<AbstractNode*>* loadImportPath(const char* name);

	std::list<AbstractNode*> locateTarget(const std::list<AbstractNode*>& nodes, const char* target);
};