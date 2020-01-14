#include "ScriptCompiler.h"

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