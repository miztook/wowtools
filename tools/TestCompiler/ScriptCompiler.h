#pragma once

#include <vector>
#include <list>

#include "varianttype.h"

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