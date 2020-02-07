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
	if (objNode->abstract)
		return;

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

	CreateMaterialScriptCompilerEvent evt(node->file.c_str(), obj->name.c_str());
	bool processed = compiler->_fireEvent(&evt, nullptr);

	bool bval;
	for (auto itr = obj->children.begin(); itr != obj->children.end(); ++itr)
	{
		const AbstractNode* node = *itr;
		if (node->type == ANT_PROPERTY)
		{
			const PropertyAbstractNode* prop = static_cast<const PropertyAbstractNode*>(node);
			switch (prop->id)
			{
			case ID_LOD_VALUES:
				for (const AbstractNode* n : prop->values)
				{
					float v;
					if (!getFloat(n, v))
					{
						ASSERT(false);
					}
				}
				break;
			case ID_LOD_DISTANCES:
				ASSERT(false);
				break;
			case ID_LOD_STRATEGY:
				if (prop->values.size() != 1)
				{
					ASSERT(false);
				}
				else
				{
					std::string strategyName;
					if (!getString(prop->values.front(), strategyName))
					{
						ASSERT(false);
					}
				}
				break;
			case ID_RECEIVE_SHADOWS:
				if (!getValue(prop, compiler, bval))
				{
					ASSERT(false);
				}
				break;
			case ID_TRANSPARENCY_CASTS_SHADOWS:
				if (!getValue(prop, compiler, bval))
				{
					ASSERT(false);
				}
				break;
			case ID_SET_TEXTURE_ALIAS:
				if (prop->values.size() != 2)
				{
					ASSERT(false);
				}
				else
				{
					const AbstractNode* i0 = getNodeAt(prop->values, 0);
					const AbstractNode* i1 = getNodeAt(prop->values, 1);
					std::string name, value;
					if (getString(i0, name) && getString(i1, value))
					{
						
					}
					else
					{
						ASSERT(false);
					}
				}
				break;
			default:
				compiler->addError(ScriptCompiler::CE_UNEXPECTEDTOKEN, prop->file.c_str(), prop->line,
					std_string_format("token \"%s\" is not recognized", prop->name.c_str()).c_str());
				break;
			}
		}
		else if (node->type == ANT_OBJECT)
		{
			processNode(compiler, *itr);
		}
	}


}

void TechniqueTranslator::translate(ScriptCompiler * compiler, const AbstractNode * node)
{
	const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);

	bool bval;
	for (auto itr = obj->children.begin(); itr != obj->children.end(); ++itr)
	{
		const AbstractNode* node = *itr;
		if (node->type == ANT_PROPERTY)
		{
			const PropertyAbstractNode* prop = static_cast<const PropertyAbstractNode*>(node);
			switch (prop->id)
			{
			case ID_SCHEME:
				if (prop->values.size() != 1)
				{
					ASSERT(false);
				}
				else
				{
					std::string scheme;
					if (!getString(prop->values.front(), scheme))
					{
						ASSERT(false);
					}
				}
				break;
			case ID_LOD_INDEX:
				if (prop->values.size() != 1)
				{
					ASSERT(false);
				}
				else
				{
					uint32_t v;
					if (!getUInt(prop->values.front(), v))
					{
						ASSERT(false);
					}
				}
				break;
			case ID_SHADOW_CASTER_MATERIAL:
				if (prop->values.size() != 1)
				{
					ASSERT(false);
				}
				else
				{
					std::string matName;
					if (!getString(prop->values.front(), matName))
					{
						ASSERT(false);
					}
				}
				break;
			case ID_GPU_VENDOR_RULE:
			case ID_GPU_DEVICE_RULE:
				ASSERT(false);
				break;
			default:
				compiler->addError(ScriptCompiler::CE_UNEXPECTEDTOKEN, prop->file.c_str(), prop->line,
					std_string_format("token \"%s\" is not recognized", prop->name.c_str()).c_str());
				break;
			}
		}
		else if (node->type == ANT_OBJECT)
		{
			processNode(compiler, *itr);
		}
	}
}

void PassTranslator::translate(ScriptCompiler* compiler, const AbstractNode* node)
{
	const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);

	for (auto itr = obj->children.begin(); itr != obj->children.end(); ++itr)
	{
		const AbstractNode* node = *itr;
		if (node->type == ANT_PROPERTY)
		{
			const PropertyAbstractNode* prop = static_cast<const PropertyAbstractNode*>(node);
			switch (prop->id)
			{
			case ID_AMBIENT:
				break;
			case ID_DIFFUSE:
				break;
			case ID_SPECULAR:
				break;
			case ID_EMISSIVE:
				break;
			case ID_SCENE_BLEND:
				break;
			case ID_SEPARATE_SCENE_BLEND:
				break;
			case ID_SCENE_BLEND_OP:
				break;
			case ID_SEPARATE_SCENE_BLEND_OP:
				break;
			case ID_DEPTH_CHECK:
				break;
			case ID_DEPTH_WRITE:
				break;
			case ID_DEPTH_BIAS:
				break;
			case ID_DEPTH_FUNC:
				break;
			case ID_ITERATION_DEPTH_BIAS:
				break;
			case ID_ALPHA_REJECTION:
				break;
			case ID_ALPHA_TO_COVERAGE:
				break;
			case ID_LIGHT_SCISSOR:
				break;
			case ID_LIGHT_CLIP_PLANES:
				break;
			case ID_TRANSPARENT_SORTING:
				break;
			case ID_ILLUMINATION_STAGE:
				break;
			case ID_CULL_HARDWARE:
				break;
			case ID_CULL_SOFTWARE:
				break;
			case ID_NORMALISE_NORMALS:
				break;
			case ID_LIGHTING:
				break;
			case ID_SHADING:
				break;
			case ID_POLYGON_MODE:
				break;
			case ID_POLYGON_MODE_OVERRIDEABLE:
				break;
			case ID_FOG_OVERRIDE:
				break;
			case ID_COLOUR_WRITE:
				break;
			case ID_MAX_LIGHTS:
				break;
			case ID_START_LIGHT:
				break;
			case ID_LIGHT_MASK:
				break;
			case ID_ITERATION:
				break;
			case ID_LINE_WIDTH:
				break;
			case ID_POINT_SIZE:
				break;
			case ID_POINT_SPRITES:
				break;
			case ID_POINT_SIZE_ATTENUATION:
				break;
			case ID_POINT_SIZE_MIN:
				break;
			case ID_POINT_SIZE_MAX:
				break;
			default:
				compiler->addError(ScriptCompiler::CE_UNEXPECTEDTOKEN, prop->file.c_str(), prop->line,
					std_string_format("token \"%s\" is not recognized", prop->name.c_str()).c_str());
				break;
			}
		}
		else if(node->type == ANT_OBJECT)
		{
			const ObjectAbstractNode* child = static_cast<const ObjectAbstractNode*>(node);
			switch (child->id)
			{
			case ID_FRAGMENT_PROGRAM_REF:
			case ID_VERTEX_PROGRAM_REF:
			case ID_GEOMETRY_PROGRAM_REF:
			case ID_TESSELLATION_HULL_PROGRAM_REF:
			case ID_TESSELLATION_DOMAIN_PROGRAM_REF:
			case ID_COMPUTE_PROGRAM_REF:
				ASSERT(false);
				break;
			case ID_SHADOW_CASTER_VERTEX_PROGRAM_REF:
				ASSERT(false);
				break;
			case ID_SHADOW_CASTER_FRAGMENT_PROGRAM_REF:
				ASSERT(false);
				break;
			case ID_SHADOW_RECEIVER_VERTEX_PROGRAM_REF:
				ASSERT(false);
				break;
			case ID_SHADOW_RECEIVER_FRAGMENT_PROGRAM_REF:
				ASSERT(false);
				break;
			case ID_FRAGMENT_PROGRAM:
			case ID_VERTEX_PROGRAM:
			case ID_GEOMETRY_PROGRAM:
			case ID_TESSELLATION_HULL_PROGRAM:
			case ID_TESSELLATION_DOMAIN_PROGRAM:
			case ID_COMPUTE_PROGRAM:
				processNode(compiler, node);
				break;
			default:
				processNode(compiler, node);
				break;
			}
		}
	}
}

void TextureUnitTranslator::translate(ScriptCompiler* compiler, const AbstractNode* node)
{
	const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);

	float fval;
	uint32_t uival;
	std::string sval;

	for (auto itr = obj->children.begin(); itr != obj->children.end(); ++itr)
	{
		const AbstractNode* node = *itr;
		if (node->type == ANT_PROPERTY)
		{
			const PropertyAbstractNode* prop = static_cast<const PropertyAbstractNode*>(node);
			switch (prop->id)
			{
			case ID_TEX_ADDRESS_MODE:
			case ID_TEX_BORDER_COLOUR:
			case ID_FILTERING:
			case ID_CMPTEST:
			case ID_CMPFUNC:
			case ID_COMP_FUNC:
			case ID_MAX_ANISOTROPY:
			case ID_MIPMAP_BIAS:
				ASSERT(false);
				break;
			case ID_SAMPLER_REF:
				if (getValue(prop, compiler, sval))
				{
				}
				break;
			case ID_TEXTURE_ALIAS:
				if (getValue(prop, compiler, sval))
				{
				}
				break;
			case ID_TEXTURE:
				ASSERT(prop->values.size() == 5);
				break;
			case ID_ANIM_TEXTURE:
				ASSERT(false);
				break;
			case ID_CUBIC_TEXTURE:
				ASSERT(false);
				break;
			case ID_TEX_COORD_SET:
				ASSERT(false);
				break;
			case ID_COLOUR_OP:
				ASSERT(false);
				break;
			case ID_COLOUR_OP_EX:
			case ID_COLOUR_OP_MULTIPASS_FALLBACK:
			case ID_ALPHA_OP_EX:
			case ID_ENV_MAP:
			case ID_SCROLL:
			case ID_SCROLL_ANIM:
			case ID_ROTATE:
			case ID_ROTATE_ANIM:
			case ID_SCALE:
			case ID_WAVE_XFORM:
			case ID_TRANSFORM:
			case ID_BINDING_TYPE:
			case ID_CONTENT_TYPE:
				ASSERT(false);
				break;
			default:
				compiler->addError(ScriptCompiler::CE_UNEXPECTEDTOKEN, prop->file.c_str(), prop->line,
					std_string_format("token \"%s\" is not recognized", prop->name.c_str()).c_str());
				break;
			}
		}
		else if (node->type == ANT_OBJECT)
		{
			processNode(compiler, node);
		}
	}
}

void SamplerTranslator::translate(ScriptCompiler* compiler, const AbstractNode* node)
{
	const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);

	for (auto itr = obj->children.begin(); itr != obj->children.end(); ++itr)
	{
		const AbstractNode* node = *itr;
		if (node->type == ANT_PROPERTY)
		{
			const PropertyAbstractNode* prop = static_cast<const PropertyAbstractNode*>(node);
			switch (prop->id)
			{
			case ID_TEX_ADDRESS_MODE:
			case ID_TEX_BORDER_COLOUR:
			case ID_FILTERING:
			case ID_CMPTEST:
			case ID_COMP_FUNC:
			case ID_MAX_ANISOTROPY:
			case ID_MIPMAP_BIAS:
				ASSERT(false);
				break;
			default:
				compiler->addError(ScriptCompiler::CE_UNEXPECTEDTOKEN, prop->file.c_str(), prop->line,
					std_string_format("token \"%s\" is not recognized", prop->name.c_str()).c_str());
				break;
			}
		}
		else if (node->type == ANT_OBJECT)
		{
			processNode(compiler, node);
		}
	}
}

void TextureSourceTranslator::translate(ScriptCompiler* compiler, const AbstractNode* node)
{
	const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);

	for (auto itr = obj->children.begin(); itr != obj->children.end(); ++itr)
	{
		const AbstractNode* node = *itr;
		if (node->type == ANT_PROPERTY)
		{
			const PropertyAbstractNode* prop = static_cast<const PropertyAbstractNode*>(node);
		}
		else
		{
			processNode(compiler, node);
		}
	}
}

void GpuProgramTranslator::translate(ScriptCompiler* compiler, const AbstractNode* node)
{
	const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);

	ASSERT_TODO
}

void SharedParamsTranslator::translate(ScriptCompiler* compiler, const AbstractNode* node)
{
	const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);

	ASSERT_TODO
}
