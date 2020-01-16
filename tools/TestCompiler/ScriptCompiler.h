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
class ScriptCompilerManager;

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
	explicit ScriptCompiler(ScriptCompilerManager* scriptCompilerManager);
	virtual ~ScriptCompiler() {}

	bool compile(const char* str, const char* source);
	bool compile(const std::list<ConcreteNode*>& nodes);
	std::list<AbstractNode*>* _generateAST(const char* str, const char* source, bool doObjects = false, bool doVariables = false);

	bool _compile(const std::list<ConcreteNode*>& nodes, bool doObjects = false, bool doVariables = false);
	void addError(uint32_t code, const char* file, int line, const char* msg = "");
	void setListener(ScriptCompilerListener* listener);
	ScriptCompilerListener* getListener() const;
	bool _fireEvent(ScriptCompilerEvent* evt, void* retVal);

	ScriptCompilerManager* getScriptCompilerManager() const { return m_scriptCompilerManager; }

private:
	std::list<AbstractNode*> convertToAST(const std::list<ConcreteNode*>& nodes);

	void processImports(const std::list<AbstractNode*>& nodes);

	std::list<AbstractNode*>* loadImportPath(const char* name);

	std::list<AbstractNode*> locateTarget(const std::list<AbstractNode*>& nodes, const char* target);

	void processObjects(std::list<AbstractNode*>& nodes, const std::list<AbstractNode*>& top);

	void processVariables(std::list<AbstractNode*>& nodes);

	void overlayObjects(const ObjectAbstractNode& source, ObjectAbstractNode& dest);

	bool isNameExcluded(const ObjectAbstractNode& node, AbstractNode* parent);

private:
	friend std::string getPropertyName(const ScriptCompiler* compiler, uint32_t id);


	std::map<std::string, std::string> m_Env;

	struct Error
	{
		std::string file;
		std::string message;
		int line;
		uint32_t code;
	};

	std::list<Error> m_Errors;

	ScriptCompilerListener* m_Listener;
	ScriptCompilerManager* m_scriptCompilerManager;
private:
	class AbstractTreeBuilder
	{
	private:
		std::list<AbstractNode*>* mNodes;
		AbstractNode* mCurrent;
		ScriptCompiler* mCompiler;
	public:
		explicit AbstractTreeBuilder(ScriptCompiler* compiler);
		std::list<AbstractNode*>* getResult() const;
		void visit(ConcreteNode* node);
		static void visit(AbstractTreeBuilder* visitor, const std::list<ConcreteNode>& nodes);
	};

	friend class AbstractTreeBuilder;
public:
	enum 
	{
		ID_ON = 1,
		ID_OFF,
		ID_TRUE,
		ID_FALSE,
		ID_YES,
		ID_NO,
	};
};

class ScriptCompilerEvent
{
public:
	std::string mType;

	ScriptCompilerEvent(const char* type) :mType(type) {}
	virtual ~ScriptCompilerEvent() {}
};

class ScriptCompilerListener
{
public:
	ScriptCompilerListener();
	virtual ~ScriptCompilerListener() {}

public:
	virtual std::list<ConcreteNode*>* importFile(ScriptCompiler* compiler, const char* name) = 0;
	virtual void preConversion(ScriptCompiler* compiler, const std::list<ConcreteNode*>& nodes);
	virtual bool postConversion(ScriptCompiler* compiler, const std::list<AbstractNode*>& nodes);
	virtual void handleError(ScriptCompiler* compiler, uint32_t code, const char* file, int line, const char* msg);
	virtual bool handleEvent(ScriptCompiler* compiler, ScriptCompilerEvent* evt, void* retval);
};

class ScriptTranslator;

class ScriptCompilerManager
{
private:
	std::vector<std::string>	m_ScriptPatterns;

	ScriptCompiler	m_ScriptCompiler;

public:
	ScriptCompilerManager();
	virtual ~ScriptCompilerManager();

	void setListener(ScriptCompilerManager* listener);
	ScriptCompilerManager* getListener();

	ScriptTranslator* getTranslator(const AbstractNode* node);

	void addScriptPattern(const char* pattern);
	const std::vector<std::string>& getScriptPatterns() const;
};