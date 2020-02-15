#pragma once

#include <vector>
#include <list>
#include <map>
#include <stdbool.h>

#include "varianttype.h"
#include "ScriptTranslator.h"

struct ConcreteNode;

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

inline const char* getAstNodeType(AbstractNodeType type)
{
	switch (type)
	{
	case ANT_ATOM:
		return "Atom";
	case ANT_OBJECT:
		return "Object";
	case ANT_PROPERTY:
		return "Property";
	case ANT_IMPORT:
		return "Import";
	case ANT_VARIABLE_SET:
		return "VariableSet";
	case ANT_VARIABLE_ACCESS:
		return "VariableAccess";
	case ANT_UNKNOWN:
	default:
		return "Unknown";
	}
}

class AbstractNode
{
public:
	std::string file;
	uint32_t line;
	AbstractNodeType type;
	AbstractNode* parent;
	VAR_T	context;

	std::list<AbstractNode*>	children;
	std::list<AbstractNode*>	values;

	static void deleteNode(const AbstractNode* node)
	{
		for (const AbstractNode* child : node->children)
		{
			deleteNode(child);
		}
		for (const AbstractNode* v : node->values)
		{
			deleteNode(v);
		}
		delete node;
	}

public:
	explicit AbstractNode(AbstractNode* _parent)
		: line(0), type(ANT_UNKNOWN), parent(_parent)
	{
	}
	virtual ~AbstractNode() {}

	virtual const char* getValue() const = 0;
};

class AtomAbstractNode : public AbstractNode
{
public:
	std::string value;
	uint32_t id;
public:
	explicit AtomAbstractNode(AbstractNode* _parent)
		: AbstractNode(_parent), id(0)
	{
		type = ANT_ATOM;
	}
	const char* getValue() const override { return value.c_str(); }
};

class ObjectAbstractNode : public AbstractNode
{
private:
	std::map<std::string, std::string> mEnv;
public:
	std::string name;
	std::string cls;
	uint32_t id;

public:
	explicit ObjectAbstractNode(AbstractNode* _parent)
		: AbstractNode(_parent), id(0)
	{
		type = ANT_OBJECT;
	}
	const char* getValue() const override { return cls.c_str(); }

	void addVariable(const char* name)
	{
		mEnv[name] = "";
	}
	void setVariable(const char* name, const char* value)
	{
		mEnv[name] = value;
	}

	std::pair<bool, std::string> getVariable(const char* name) const;
	const std::map<std::string, std::string>& getVariables() const { return mEnv; }
};

class PropertyAbstractNode : public AbstractNode
{
public:
	std::string name;
	uint32_t id;
public:
	explicit PropertyAbstractNode(AbstractNode* _parent)
		: AbstractNode(_parent), id(0)
	{
		type = ANT_PROPERTY;
	}
	const char* getValue() const override { return name.c_str(); }
};

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

	struct Error
	{
		std::string file;
		std::string message;
		int line;
		uint32_t code;
	};

public:
	explicit ScriptCompiler(ScriptCompilerManager* scriptCompilerManager);
	virtual ~ScriptCompiler() {}

	bool compile(const char* str, const char* source);
	bool compile(const std::list<ConcreteNode*>& nodes);

	void addError(uint32_t code, const char* file, int line, const char* msg = "");
	ScriptCompilerManager* getScriptCompilerManager() const { return m_scriptCompilerManager; }

	const std::list<Error>& getErrorList() const { return m_Errors; }

	std::list<AbstractNode*> convertToAST(const std::list<ConcreteNode*>& nodes);

private:
	void initWordMap();

private:
	friend std::string getPropertyName(const ScriptCompiler* compiler, uint32_t id);

	std::map<std::string, uint32_t> m_Ids;
	std::list<Error> m_Errors;
	ScriptCompilerManager* m_scriptCompilerManager;

private:
	class AbstractTreeBuilder
	{
	private:
		std::list<AbstractNode*> mNodes;
		AbstractNode* mCurrent;
		ScriptCompiler* mCompiler;
	public:
		explicit AbstractTreeBuilder(ScriptCompiler* compiler);
		std::list<AbstractNode*> getResult() { return mNodes; }
		void visit(ConcreteNode* node);
		static void visit(AbstractTreeBuilder* visitor, const std::list<ConcreteNode*>& nodes);
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

class ScriptCompilerManager
{
private:
	ScriptCompiler	m_ScriptCompiler;

public:
	ScriptCompilerManager();
	virtual ~ScriptCompilerManager();

	ScriptTranslator* getTranslator(const AbstractNode* node);

	bool parseScript(const char* str, const char* source);
	std::list<AbstractNode*> convertToAST(const std::list<ConcreteNode*>& nodes);

private:
	MaterialTranslator	m_MaterialTranslator;
	TechniqueTranslator		m_TechniqueTranslator;
	PassTranslator	m_PassTranslator;
	TextureUnitTranslator	m_TextureUnitTranslator;
	SamplerTranslator	m_SamplerTranslator;
	TextureSourceTranslator		m_TextureSourceTranslator;
	GpuProgramTranslator	m_GpuProgramTranslator;
};

enum
{	
	ID_MATERIAL = 3,
	ID_PASS,


};