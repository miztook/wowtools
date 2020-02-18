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

void ScriptTranslator::processNode(ScriptCompiler* compiler, AbstractNode* node)
{
	if (node->type != ANT_OBJECT)
		return;

	ObjectAbstractNode* objNode = static_cast<ObjectAbstractNode*>(node);

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

bool ScriptTranslator::getRenderQueue(const AbstractNode* node, int& queue)
{
	if (node->type != ANT_ATOM)
		return false;
	const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(node);
	switch (atom->id)
	{
	case ID_BACKGROUND:
		queue = ERQ_BACKGROUND;
		break;
	case ID_GEOMETRY:
		queue = ERQ_GEOMETRY;
		break;
	case ID_ALPHATEST:
		queue = ERQ_ALPHATEST;
		break;
	case ID_GEOMETRYLAST:
		queue = ERQ_GEOMETRYLAST;
		break;
	case ID_TRANSPARENT:
		queue = ERQ_TRANSPARENT;
		break;
	case ID_OVERLAY:
		queue = ERQ_OVERLAY;
		break;
	default:
		ASSERT(false);
		break;
	}
	return true;
}

bool ScriptTranslator::getLightMode(const AbstractNode* node, E_LIGHT_MODE& lightmode)
{
	if (node->type != ANT_ATOM)
		return false;
	const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(node);
	switch (atom->id)
	{
	case ID_ALWAYS:
		lightmode = ELM_ALWAYS;
		break;
	case ID_FORWARD_BASE:
		lightmode = ELM_FORWARD_BASE;
		break;
	case ID_FORWARD_ADD:
		lightmode = ELM_FORWARD_ADD;
		break;
	default:
		ASSERT(false);
		break;
	}
	return true;
}

bool ScriptTranslator::getCull(const AbstractNode* node, E_CULL_MODE& mode)
{
	if (node->type != ANT_ATOM)
		return false;
	const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(node);
	switch (atom->id)
	{
	case ID_BACK:
		mode = ECM_BACK;
		break;
	case ID_FRONT:
		mode = ECM_FRONT;
		break;
	case ID_NONE:
		mode = ECM_NONE;
		break;
	default:
		ASSERT(false);
		break;
	}
	return true;
}

bool ScriptTranslator::getAntiAliasing(const AbstractNode* node, E_ANTI_ALIASING_MODE& mode)
{
	if (node->type != ANT_ATOM)
		return false;
	const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(node);
	switch (atom->id)
	{
	case ID_OFF:
		mode = EAAM_OFF;
		break;
	case ID_SIMPLE:
		mode = EAAM_SIMPLE;
		break;
	case ID_LINE_SMOOTH:
		mode = EAAM_LINE_SMOOTH;
		break;
	default:
		ASSERT(false);
		break;
	}
	return true;
}

bool ScriptTranslator::getComparisonFunc(const AbstractNode* node, E_COMPARISON_FUNC& func)
{
	if (node->type != ANT_ATOM)
		return false;
	const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(node);
	switch (atom->id)
	{
	case ID_NEVER:
		func = ECFN_NEVER;
		break;
	case ID_LESSEQUAL:
		func = ECFN_LESSEQUAL;
		break;
	case ID_EQUAL:
		func = ECFN_EQUAL;
		break;
	case ID_LESS:
		func = ECFN_LESS;
		break;
	case ID_NOTEQUAL:
		func = ECFN_NOTEQUAL;
		break;
	case ID_GREATEREQUAL:
		func = ECFN_GREATEREQUAL;
		break;
	case ID_GREATER:
		func = ECFN_GREATER;
		break;
	case ID_ALWAYS:
		func = ECFN_ALWAYS;
		break;
	default:
		ASSERT(false);
		break;
	}
	return true;
}

bool ScriptTranslator::getColorMask(const AbstractNode* node, E_COLOR_MASK& colormask)
{
	if (node->type != ANT_ATOM)
		return false;
	const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(node);
	switch (atom->id)
	{
	case ID_A:
		colormask = COLORWRITE_ALPHA;
		break;
	case ID_R:
		colormask = COLORWRITE_RED;
		break;
	case ID_G:
		colormask = COLORWRITE_GREEN;
		break;
	case ID_B:
		colormask = COLORWRITE_BLUE;
		break;
	case ID_RGBA:
		colormask = COLORWRITE_ALL;
		break;
	default:
		ASSERT(false);
		break;
	}
	return true;
}

bool ScriptTranslator::getBlendFactor(const AbstractNode* node, E_BLEND_FACTOR& blend)
{
	if (node->type != ANT_ATOM)
		return false;
	const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(node);
	switch (atom->id)
	{
	case ID_ZERO:
		blend = EBF_ZERO;
		break;
	case ID_ONE:
		blend = EBF_ONE;
		break;
	case ID_DST_COLOR:
		blend = EBF_DST_COLOR;
		break;
	case ID_ONE_MINUS_DST_COLOR:
		blend = EBF_ONE_MINUS_DST_COLOR;
		break;
	case ID_SRC_COLOR:
		blend = EBF_SRC_COLOR;
		break;
	case ID_ONE_MINUS_SRC_COLOR:
		blend = EBF_ONE_MINUS_SRC_COLOR;
		break;
	case ID_DST_ALPHA:
		blend = EBF_DST_ALPHA;
		break;
	case ID_ONE_MINUS_DST_ALPHA:
		blend = EBF_ONE_MINUS_DST_ALPHA;
		break;
	case ID_SRC_ALPHA:
		blend = EBF_SRC_ALPHA;
		break;
	case ID_ONE_MINUS_SRC_ALPHA:
		blend = EBF_ONE_MINUS_SRC_ALPHA;
		break;
	default:
		ASSERT(false);
		break;
	}
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

void MaterialTranslator::translate(ScriptCompiler* compiler, AbstractNode* node)
{
	ObjectAbstractNode* obj = static_cast<ObjectAbstractNode*>(node);
	if (obj->name.empty())
		compiler->addError(ScriptCompiler::CE_OBJECTNAMEEXPECTED, obj->file.c_str(), obj->line);

	ASSERT(!obj->context && obj->cls == "Material");

	CMaterial* material = nullptr;
	auto itr = m_MaterialMap.find(obj->name);
	if (itr != m_MaterialMap.end())
	{
		material = itr->second;
	}
	else
	{
		material = new CMaterial(obj->name.c_str());
		m_MaterialMap[obj->name] = material;
	}
	obj->context = material;

	bool bval;
	int ival;
	for (auto itr = obj->children.begin(); itr != obj->children.end(); ++itr)
	{
		AbstractNode* child = *itr;

		if (child->type == ANT_PROPERTY)			//解析属性
		{
			const PropertyAbstractNode* prop = static_cast<const PropertyAbstractNode*>(child);
			switch (prop->id)
			{
			case ID_QUEUE:
				if (!getRenderQueue(prop->values.front(), material->RenderQueue))
					ASSERT(false);
				break;
			default:
				compiler->addError(ScriptCompiler::CE_UNEXPECTEDTOKEN, prop->file.c_str(), prop->line,
					std_string_format("token \"%s\" is not recognized", prop->name.c_str()).c_str());
				break;
			}
		}
		else if (child->type == ANT_OBJECT)
		{
			processNode(compiler, child);			//继续子node
		}
	}
}

void PassTranslator::translate(ScriptCompiler* compiler, AbstractNode* node)
{
	const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);

	CMaterial* material = static_cast<CMaterial*>(obj->parent->context);
	CPass* m_Pass = material->addPass(ELM_ALWAYS);

	for (auto itr = obj->children.begin(); itr != obj->children.end(); ++itr)
	{
		AbstractNode* child = *itr;

		if (child->type == ANT_PROPERTY)			//解析属性
		{
			const PropertyAbstractNode* prop = static_cast<const PropertyAbstractNode*>(child);
			switch (prop->id)
			{
			case ID_LIGHT_MODE:
				if (!getLightMode(prop->values.front(), m_Pass->LightMode))
					ASSERT(false);
				break;
			case ID_CULL:
				if (!getCull(prop->values.front(), m_Pass->Cull))
					ASSERT(false);
				break;
			case ID_ANTIALIASING:
				if (!getAntiAliasing(prop->values.front(), m_Pass->AntiAliasing))
					ASSERT(false);
				break;
			case ID_ZTEST:
				if (!getComparisonFunc(prop->values.front(), m_Pass->ZTest))
					ASSERT(false);
				break;
			case ID_ZWRITE:
				if (!getBoolean(prop->values.front(), m_Pass->ZWrite))
					ASSERT(false);
				break;
			case ID_COLOR_MASK:
				if (!getColorMask(prop->values.front(), m_Pass->ColorMask))
					ASSERT(false);
				break;
			case ID_ALPHABLEND:
				if (prop->values.size() == 1)
				{
					if (!getBoolean(prop->values.front(), m_Pass->AlphaBlendEnabled))
						ASSERT(false);
					ASSERT(m_Pass->AlphaBlendEnabled == false);
				}
				else if (prop->values.size() == 2)
				{
					if (!getBlendFactor(prop->values.front(), m_Pass->SrcBlend) ||
						!getBlendFactor(prop->values.back(), m_Pass->DestBlend))
						ASSERT(false);
					m_Pass->AlphaBlendEnabled = true;
				}
				else
				{
					ASSERT(false);
				}
				break;
			case ID_DEFINE:
				for (const AbstractNode* node : prop->values)
				{
					if (node->type == ANT_ATOM)
					{
						std::string macro;
						if (getString(node, macro))
							m_Pass->addMacro(macro.c_str());
					}
				}
				break;
			case ID_VSFILE:
				if (!getString(prop->values.front(), m_Pass->VSFile))
					ASSERT(false);
				break;
			case ID_PSFILE:
				if (!getString(prop->values.front(), m_Pass->PSFile))
					ASSERT(false);
				break;
			default:
				compiler->addError(ScriptCompiler::CE_UNEXPECTEDTOKEN, prop->file.c_str(), prop->line,
					std_string_format("token \"%s\" is not recognized", prop->name.c_str()).c_str());
				break;
			}
		}
		else if (child->type == ANT_ATOM)
		{
			const AtomAbstractNode* atom = static_cast<const AtomAbstractNode*>(child);
			switch (atom->id)
			{
			case ID_DEFINE:
				break;
			case ID_VSFILE:
				m_Pass->VSFile = "";
				break;
			case ID_PSFILE:
				m_Pass->PSFile = "";
				break;
			default:
				compiler->addError(ScriptCompiler::CE_UNEXPECTEDTOKEN, atom->file.c_str(), atom->line,
					std_string_format("token \"%s\" is not recognized", atom->getValue()).c_str());
				break;
			}
		}
		else
		{
			ASSERT(false);
		}
	}
}