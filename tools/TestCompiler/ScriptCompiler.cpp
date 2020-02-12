#include "ScriptCompiler.h"
#include "base.h"
#include "ScriptParser.h"
#include "ScriptTranslator.h"
#include "stringext.h"

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

ScriptCompiler::ScriptCompiler(ScriptCompilerManager* scriptCompilerManager)
	: m_Listener(nullptr), m_scriptCompilerManager(scriptCompilerManager)
{
	initWordMap();
}

bool ScriptCompiler::compile(const char* str, const char* source)
{
	std::string error;
	std::vector<ScriptToken> tokens = ScriptLexer::tokenize(str, source, error);

	if (!error.empty())
	{
		ASSERT(false);
		return false;
	}

	std::list<ConcreteNode*> nodes = ScriptParser::parse(tokens);
	return compile(nodes);
}

bool ScriptCompiler::compile(const std::list<ConcreteNode*>& nodes)
{
	m_Errors.clear();
	m_Env.clear();

	if (m_Listener)
		m_Listener->preConversion(this, nodes);

	std::list<AbstractNode*> astList = convertToAST(nodes);

	processObjects(astList, astList);

	processVariables(astList);

	if (m_Listener && !m_Listener->postConversion(this, astList))
		return m_Errors.empty();

	for (const AbstractNode* node : astList)
	{
		if (node->type == ANT_OBJECT && static_cast<const ObjectAbstractNode*>(node)->abstract)
			continue;

		ScriptTranslator* translator = m_scriptCompilerManager->getTranslator(node);
		if (translator)
			translator->translate(this, node);
	}

	for (AbstractNode* node : astList)
	{
		delete node;
	}
	astList.clear();

	return m_Errors.empty();
}

void ScriptCompiler::addError(uint32_t code, const char* file, int line, const char* msg /*= ""*/)
{
	if (m_Listener)
	{
		m_Listener->handleError(this, code, file, line, msg);
	}
	else
	{
		m_defaultListener.handleError(this, code, file, line, msg);
	}

	ScriptCompiler::Error err;
	err.file = file;
	err.message = msg;
	err.line = line;
	err.code = code;
	m_Errors.push_back(err);
}

bool ScriptCompiler::_fireEvent(ScriptCompilerEvent* evt, void* retVal)
{
	if (m_Listener)
		return m_Listener->handleEvent(this, evt, retVal);
	return false;
}

std::list<AbstractNode*> ScriptCompiler::convertToAST(const std::list<ConcreteNode*>& nodes)
{
	AbstractTreeBuilder builder(this);
	AbstractTreeBuilder::visit(&builder, nodes);
	return builder.getResult();
}

void ScriptCompiler::processImports(std::list<AbstractNode*>& nodes)
{
	ASSERT_TODO
}

std::list<AbstractNode*>* ScriptCompiler::loadImportPath(const char* name)
{
	ASSERT_TODO
	return nullptr;
}

std::list<AbstractNode*> ScriptCompiler::locateTarget(const std::list<AbstractNode*>& nodes, const char* target)
{
	ASSERT_TODO
	return std::list<AbstractNode*>();
}

void ScriptCompiler::processObjects(std::list<AbstractNode*>& nodes, const std::list<AbstractNode*>& top)
{
	for (auto itr = nodes.begin(); itr != nodes.end(); ++itr)
	{
		if ((*itr)->type != ANT_OBJECT)
			continue;

		ObjectAbstractNode* obj = static_cast<ObjectAbstractNode*>(*itr);

		for (const auto& base : obj->bases)
		{
			std::list<AbstractNode*> newNodes = locateTarget(top, base.c_str());
			if (newNodes.empty())
				addError(CE_OBJECTBASENOTFOUND, obj->file.c_str(), obj->line, base.c_str());

			for (const auto& n : newNodes)
			{
				if (n->type != ANT_OBJECT)
					continue;
				ASSERT_TODO
			}
		}

		processObjects(obj->children, top);

		obj->children.insert(obj->children.begin(), obj->overrides.begin(), obj->overrides.end());
	}
}

void ScriptCompiler::processVariables(std::list<AbstractNode*>& nodes)
{
	auto i = nodes.begin();
	while (i != nodes.end())
	{
		auto cur = i;
		AbstractNode* node = *cur;
		++i;

		if (node->type == ANT_OBJECT)
		{
			ObjectAbstractNode* obj = static_cast<ObjectAbstractNode*>(node);
			if (!obj->abstract)
			{
				processVariables(obj->children);
				processVariables(obj->values);
			}
		}
		else if (node->type == ANT_PROPERTY)
		{
			PropertyAbstractNode* prop = static_cast<PropertyAbstractNode*>(node);
			processVariables(prop->values);
		}
		else if (node->type == ANT_VARIABLE_ACCESS)
		{
			ASSERT_TODO
		}
	}
}

bool ScriptCompiler::isNameExcluded(const ObjectAbstractNode& node, const AbstractNode* parent)
{
	if (node.id == ID_PASS)
	{
		while (parent && parent->type == ANT_OBJECT)
		{
			const ObjectAbstractNode *obj = static_cast<const ObjectAbstractNode*>(parent);
			if (obj->id == ID_COMPOSITOR)
				return true;
			parent = obj->parent;
		}
	}
	else if (node.id == ID_TEXTURE_SOURCE)
	{
		while (parent && parent->type == ANT_OBJECT)
		{
			const ObjectAbstractNode *obj = static_cast<const ObjectAbstractNode*>(parent);
			if (obj->id == ID_TEXTURE_UNIT)
				return true;
			parent = obj->parent;
		}
	}
	return false;
}

void ScriptCompiler::initWordMap()
{
	m_Ids["on"] = ID_ON;
	m_Ids["off"] = ID_OFF;
	m_Ids["true"] = ID_TRUE;
	m_Ids["false"] = ID_FALSE;
	m_Ids["yes"] = ID_YES;
	m_Ids["no"] = ID_NO;

	// Material ids
	m_Ids["material"] = ID_MATERIAL;
	m_Ids["vertex_program"] = ID_VERTEX_PROGRAM;
	m_Ids["geometry_program"] = ID_GEOMETRY_PROGRAM;
	m_Ids["fragment_program"] = ID_FRAGMENT_PROGRAM;
	m_Ids["tessellation_hull_program"] = ID_TESSELLATION_HULL_PROGRAM;
	m_Ids["tessellation_domain_program"] = ID_TESSELLATION_DOMAIN_PROGRAM;
	m_Ids["compute_program"] = ID_COMPUTE_PROGRAM;
	m_Ids["technique"] = ID_TECHNIQUE;
	m_Ids["pass"] = ID_PASS;
	m_Ids["texture_unit"] = ID_TEXTURE_UNIT;
	m_Ids["vertex_program_ref"] = ID_VERTEX_PROGRAM_REF;
	m_Ids["geometry_program_ref"] = ID_GEOMETRY_PROGRAM_REF;
	m_Ids["fragment_program_ref"] = ID_FRAGMENT_PROGRAM_REF;
	m_Ids["tessellation_hull_program_ref"] = ID_TESSELLATION_HULL_PROGRAM_REF;
	m_Ids["tessellation_domain_program_ref"] = ID_TESSELLATION_DOMAIN_PROGRAM_REF;
	m_Ids["compute_program_ref"] = ID_COMPUTE_PROGRAM_REF;
	m_Ids["shadow_caster_vertex_program_ref"] = ID_SHADOW_CASTER_VERTEX_PROGRAM_REF;
	m_Ids["shadow_caster_fragment_program_ref"] = ID_SHADOW_CASTER_FRAGMENT_PROGRAM_REF;
	m_Ids["shadow_receiver_vertex_program_ref"] = ID_SHADOW_RECEIVER_VERTEX_PROGRAM_REF;
	m_Ids["shadow_receiver_fragment_program_ref"] = ID_SHADOW_RECEIVER_FRAGMENT_PROGRAM_REF;

	m_Ids["lod_values"] = ID_LOD_VALUES;
	m_Ids["lod_strategy"] = ID_LOD_STRATEGY;
	m_Ids["lod_distances"] = ID_LOD_DISTANCES;
	m_Ids["receive_shadows"] = ID_RECEIVE_SHADOWS;
	m_Ids["transparency_casts_shadows"] = ID_TRANSPARENCY_CASTS_SHADOWS;
	m_Ids["set_texture_alias"] = ID_SET_TEXTURE_ALIAS;

	m_Ids["source"] = ID_SOURCE;
	m_Ids["syntax"] = ID_SYNTAX;
	m_Ids["default_params"] = ID_DEFAULT_PARAMS;
	m_Ids["param_indexed"] = ID_PARAM_INDEXED;
	m_Ids["param_named"] = ID_PARAM_NAMED;
	m_Ids["param_indexed_auto"] = ID_PARAM_INDEXED_AUTO;
	m_Ids["param_named_auto"] = ID_PARAM_NAMED_AUTO;

	m_Ids["scheme"] = ID_SCHEME;
	m_Ids["lod_index"] = ID_LOD_INDEX;
	m_Ids["shadow_caster_material"] = ID_SHADOW_CASTER_MATERIAL;
	m_Ids["shadow_receiver_material"] = ID_SHADOW_RECEIVER_MATERIAL;
	m_Ids["gpu_vendor_rule"] = ID_GPU_VENDOR_RULE;
	m_Ids["gpu_device_rule"] = ID_GPU_DEVICE_RULE;
	m_Ids["include"] = ID_INCLUDE;
	m_Ids["exclude"] = ID_EXCLUDE;

	m_Ids["ambient"] = ID_AMBIENT;
	m_Ids["diffuse"] = ID_DIFFUSE;
	m_Ids["specular"] = ID_SPECULAR;
	m_Ids["emissive"] = ID_EMISSIVE;
	m_Ids["vertexcolour"] = ID_VERTEXCOLOUR;
	m_Ids["scene_blend"] = ID_SCENE_BLEND;
	m_Ids["colour_blend"] = ID_COLOUR_BLEND;
	m_Ids["one"] = ID_ONE;
	m_Ids["zero"] = ID_ZERO;
	m_Ids["dest_colour"] = ID_DEST_COLOUR;
	m_Ids["src_colour"] = ID_SRC_COLOUR;
	m_Ids["one_minus_src_colour"] = ID_ONE_MINUS_SRC_COLOUR;
	m_Ids["one_minus_dest_colour"] = ID_ONE_MINUS_DEST_COLOUR;
	m_Ids["dest_alpha"] = ID_DEST_ALPHA;
	m_Ids["src_alpha"] = ID_SRC_ALPHA;
	m_Ids["one_minus_dest_alpha"] = ID_ONE_MINUS_DEST_ALPHA;
	m_Ids["one_minus_src_alpha"] = ID_ONE_MINUS_SRC_ALPHA;
	m_Ids["separate_scene_blend"] = ID_SEPARATE_SCENE_BLEND;
	m_Ids["scene_blend_op"] = ID_SCENE_BLEND_OP;
	m_Ids["reverse_subtract"] = ID_REVERSE_SUBTRACT;
	m_Ids["min"] = ID_MIN;
	m_Ids["max"] = ID_MAX;
	m_Ids["separate_scene_blend_op"] = ID_SEPARATE_SCENE_BLEND_OP;
	m_Ids["depth_check"] = ID_DEPTH_CHECK;
	m_Ids["depth_write"] = ID_DEPTH_WRITE;
	m_Ids["depth_func"] = ID_DEPTH_FUNC;
	m_Ids["depth_bias"] = ID_DEPTH_BIAS;
	m_Ids["iteration_depth_bias"] = ID_ITERATION_DEPTH_BIAS;
	m_Ids["always_fail"] = ID_ALWAYS_FAIL;
	m_Ids["always_pass"] = ID_ALWAYS_PASS;
	m_Ids["less_equal"] = ID_LESS_EQUAL;
	m_Ids["less"] = ID_LESS;
	m_Ids["equal"] = ID_EQUAL;
	m_Ids["not_equal"] = ID_NOT_EQUAL;
	m_Ids["greater_equal"] = ID_GREATER_EQUAL;
	m_Ids["greater"] = ID_GREATER;
	m_Ids["alpha_rejection"] = ID_ALPHA_REJECTION;
	m_Ids["alpha_to_coverage"] = ID_ALPHA_TO_COVERAGE;
	m_Ids["light_scissor"] = ID_LIGHT_SCISSOR;
	m_Ids["light_clip_planes"] = ID_LIGHT_CLIP_PLANES;
	m_Ids["transparent_sorting"] = ID_TRANSPARENT_SORTING;
	m_Ids["illumination_stage"] = ID_ILLUMINATION_STAGE;
	m_Ids["decal"] = ID_DECAL;
	m_Ids["cull_hardware"] = ID_CULL_HARDWARE;
	m_Ids["clockwise"] = ID_CLOCKWISE;
	m_Ids["anticlockwise"] = ID_ANTICLOCKWISE;
	m_Ids["cull_software"] = ID_CULL_SOFTWARE;
	m_Ids["back"] = ID_BACK;
	m_Ids["front"] = ID_FRONT;
	m_Ids["normalise_normals"] = ID_NORMALISE_NORMALS;
	m_Ids["lighting"] = ID_LIGHTING;
	m_Ids["shading"] = ID_SHADING;
	m_Ids["flat"] = ID_FLAT;
	m_Ids["gouraud"] = ID_GOURAUD;
	m_Ids["phong"] = ID_PHONG;
	m_Ids["polygon_mode"] = ID_POLYGON_MODE;
	m_Ids["solid"] = ID_SOLID;
	m_Ids["wireframe"] = ID_WIREFRAME;
	m_Ids["points"] = ID_POINTS;
	m_Ids["polygon_mode_overrideable"] = ID_POLYGON_MODE_OVERRIDEABLE;
	m_Ids["fog_override"] = ID_FOG_OVERRIDE;
	m_Ids["none"] = ID_NONE;
	m_Ids["linear"] = ID_LINEAR;
	m_Ids["exp"] = ID_EXP;
	m_Ids["exp2"] = ID_EXP2;
	m_Ids["colour_write"] = ID_COLOUR_WRITE;
	m_Ids["max_lights"] = ID_MAX_LIGHTS;
	m_Ids["start_light"] = ID_START_LIGHT;
	m_Ids["iteration"] = ID_ITERATION;
	m_Ids["once"] = ID_ONCE;
	m_Ids["once_per_light"] = ID_ONCE_PER_LIGHT;
	m_Ids["per_n_lights"] = ID_PER_N_LIGHTS;
	m_Ids["per_light"] = ID_PER_LIGHT;
	m_Ids["point"] = ID_POINT;
	m_Ids["spot"] = ID_SPOT;
	m_Ids["directional"] = ID_DIRECTIONAL;
	m_Ids["light_mask"] = ID_LIGHT_MASK;
	m_Ids["point_size"] = ID_POINT_SIZE;
	m_Ids["point_sprites"] = ID_POINT_SPRITES;
	m_Ids["point_size_min"] = ID_POINT_SIZE_MIN;
	m_Ids["point_size_max"] = ID_POINT_SIZE_MAX;
	m_Ids["point_size_attenuation"] = ID_POINT_SIZE_ATTENUATION;

	m_Ids["texture_alias"] = ID_TEXTURE_ALIAS;
	m_Ids["texture"] = ID_TEXTURE;
	m_Ids["1d"] = ID_1D;
	m_Ids["2d"] = ID_2D;
	m_Ids["3d"] = ID_3D;
	m_Ids["cubic"] = ID_CUBIC;
	m_Ids["unlimited"] = ID_UNLIMITED;
	m_Ids["2darray"] = ID_2DARRAY;
	m_Ids["alpha"] = ID_ALPHA;
	m_Ids["gamma"] = ID_GAMMA;
	m_Ids["anim_texture"] = ID_ANIM_TEXTURE;
	m_Ids["cubic_texture"] = ID_CUBIC_TEXTURE;
	m_Ids["separateUV"] = ID_SEPARATE_UV;
	m_Ids["combinedUVW"] = ID_COMBINED_UVW;
	m_Ids["tex_coord_set"] = ID_TEX_COORD_SET;
	m_Ids["tex_address_mode"] = ID_TEX_ADDRESS_MODE;
	m_Ids["wrap"] = ID_WRAP;
	m_Ids["clamp"] = ID_CLAMP;
	m_Ids["mirror"] = ID_MIRROR;
	m_Ids["border"] = ID_BORDER;
	m_Ids["tex_border_colour"] = ID_TEX_BORDER_COLOUR;
	m_Ids["filtering"] = ID_FILTERING;
	m_Ids["bilinear"] = ID_BILINEAR;
	m_Ids["trilinear"] = ID_TRILINEAR;
	m_Ids["anisotropic"] = ID_ANISOTROPIC;
	m_Ids["compare_test"] = ID_CMPTEST;
	m_Ids["compare_func"] = ID_CMPFUNC;
	m_Ids["max_anisotropy"] = ID_MAX_ANISOTROPY;
	m_Ids["mipmap_bias"] = ID_MIPMAP_BIAS;
	m_Ids["colour_op"] = ID_COLOUR_OP;
	m_Ids["replace"] = ID_REPLACE;
	m_Ids["add"] = ID_ADD;
	m_Ids["modulate"] = ID_MODULATE;
	m_Ids["alpha_blend"] = ID_ALPHA_BLEND;
	m_Ids["colour_op_ex"] = ID_COLOUR_OP_EX;
	m_Ids["source1"] = ID_SOURCE1;
	m_Ids["source2"] = ID_SOURCE2;
	m_Ids["modulate"] = ID_MODULATE;
	m_Ids["modulate_x2"] = ID_MODULATE_X2;
	m_Ids["modulate_x4"] = ID_MODULATE_X4;
	m_Ids["add"] = ID_ADD;
	m_Ids["add_signed"] = ID_ADD_SIGNED;
	m_Ids["add_smooth"] = ID_ADD_SMOOTH;
	m_Ids["subtract"] = ID_SUBTRACT;
	m_Ids["blend_diffuse_alpha"] = ID_BLEND_DIFFUSE_ALPHA;
	m_Ids["blend_texture_alpha"] = ID_BLEND_TEXTURE_ALPHA;
	m_Ids["blend_current_alpha"] = ID_BLEND_CURRENT_ALPHA;
	m_Ids["blend_manual"] = ID_BLEND_MANUAL;
	m_Ids["dotproduct"] = ID_DOT_PRODUCT;
	m_Ids["blend_diffuse_colour"] = ID_BLEND_DIFFUSE_COLOUR;
	m_Ids["src_current"] = ID_SRC_CURRENT;
	m_Ids["src_texture"] = ID_SRC_TEXTURE;
	m_Ids["src_diffuse"] = ID_SRC_DIFFUSE;
	m_Ids["src_specular"] = ID_SRC_SPECULAR;
	m_Ids["src_manual"] = ID_SRC_MANUAL;
	m_Ids["colour_op_multipass_fallback"] = ID_COLOUR_OP_MULTIPASS_FALLBACK;
	m_Ids["alpha_op_ex"] = ID_ALPHA_OP_EX;
	m_Ids["env_map"] = ID_ENV_MAP;
	m_Ids["spherical"] = ID_SPHERICAL;
	m_Ids["planar"] = ID_PLANAR;
	m_Ids["cubic_reflection"] = ID_CUBIC_REFLECTION;
	m_Ids["cubic_normal"] = ID_CUBIC_NORMAL;
	m_Ids["scroll"] = ID_SCROLL;
	m_Ids["scroll_anim"] = ID_SCROLL_ANIM;
	m_Ids["rotate"] = ID_ROTATE;
	m_Ids["rotate_anim"] = ID_ROTATE_ANIM;
	m_Ids["scale"] = ID_SCALE;
	m_Ids["wave_xform"] = ID_WAVE_XFORM;
	m_Ids["scroll_x"] = ID_SCROLL_X;
	m_Ids["scroll_y"] = ID_SCROLL_Y;
	m_Ids["scale_x"] = ID_SCALE_X;
	m_Ids["scale_y"] = ID_SCALE_Y;
	m_Ids["sine"] = ID_SINE;
	m_Ids["triangle"] = ID_TRIANGLE;
	m_Ids["sawtooth"] = ID_SAWTOOTH;
	m_Ids["square"] = ID_SQUARE;
	m_Ids["inverse_sawtooth"] = ID_INVERSE_SAWTOOTH;
	m_Ids["transform"] = ID_TRANSFORM;
	m_Ids["binding_type"] = ID_BINDING_TYPE;
	m_Ids["vertex"] = ID_VERTEX;
	m_Ids["fragment"] = ID_FRAGMENT;
	m_Ids["geometry"] = ID_GEOMETRY;
	m_Ids["tessellation_hull"] = ID_TESSELLATION_HULL;
	m_Ids["tessellation_domain"] = ID_TESSELLATION_DOMAIN;
	m_Ids["compute"] = ID_COMPUTE;
	m_Ids["content_type"] = ID_CONTENT_TYPE;
	m_Ids["named"] = ID_NAMED;
	m_Ids["shadow"] = ID_SHADOW;
	m_Ids["texture_source"] = ID_TEXTURE_SOURCE;
	m_Ids["shared_params"] = ID_SHARED_PARAMS;
	m_Ids["shared_param_named"] = ID_SHARED_PARAM_NAMED;
	m_Ids["shared_params_ref"] = ID_SHARED_PARAMS_REF;

	// Particle system
	m_Ids["particle_system"] = ID_PARTICLE_SYSTEM;
	m_Ids["emitter"] = ID_EMITTER;
	m_Ids["affector"] = ID_AFFECTOR;

	// Compositor
	m_Ids["compositor"] = ID_COMPOSITOR;
	m_Ids["target"] = ID_TARGET;
	m_Ids["target_output"] = ID_TARGET_OUTPUT;

	m_Ids["input"] = ID_INPUT;
	//m_Ids["none"] = ID_NONE; - already registered
	m_Ids["previous"] = ID_PREVIOUS;
	m_Ids["target_width"] = ID_TARGET_WIDTH;
	m_Ids["target_height"] = ID_TARGET_HEIGHT;
	m_Ids["target_width_scaled"] = ID_TARGET_WIDTH_SCALED;
	m_Ids["target_height_scaled"] = ID_TARGET_HEIGHT_SCALED;
	m_Ids["pooled"] = ID_POOLED;
	//m_Ids["gamma"] = ID_GAMMA; - already registered
	m_Ids["no_fsaa"] = ID_NO_FSAA;
	m_Ids["depth_pool"] = ID_DEPTH_POOL;

	m_Ids["texture_ref"] = ID_TEXTURE_REF;
	m_Ids["local_scope"] = ID_SCOPE_LOCAL;
	m_Ids["chain_scope"] = ID_SCOPE_CHAIN;
	m_Ids["global_scope"] = ID_SCOPE_GLOBAL;
	m_Ids["compositor_logic"] = ID_COMPOSITOR_LOGIC;

	m_Ids["only_initial"] = ID_ONLY_INITIAL;
	m_Ids["visibility_mask"] = ID_VISIBILITY_MASK;
	m_Ids["lod_bias"] = ID_LOD_BIAS;
	m_Ids["material_scheme"] = ID_MATERIAL_SCHEME;
	m_Ids["shadows"] = ID_SHADOWS_ENABLED;

	m_Ids["clear"] = ID_CLEAR;
	m_Ids["stencil"] = ID_STENCIL;
	m_Ids["render_scene"] = ID_RENDER_SCENE;
	m_Ids["render_quad"] = ID_RENDER_QUAD;
	m_Ids["identifier"] = ID_IDENTIFIER;
	m_Ids["first_render_queue"] = ID_FIRST_RENDER_QUEUE;
	m_Ids["last_render_queue"] = ID_LAST_RENDER_QUEUE;
	m_Ids["quad_normals"] = ID_QUAD_NORMALS;
	m_Ids["camera_far_corners_view_space"] = ID_CAMERA_FAR_CORNERS_VIEW_SPACE;
	m_Ids["camera_far_corners_world_space"] = ID_CAMERA_FAR_CORNERS_WORLD_SPACE;

	m_Ids["buffers"] = ID_BUFFERS;
	m_Ids["colour"] = ID_COLOUR;
	m_Ids["depth"] = ID_DEPTH;
	m_Ids["colour_value"] = ID_COLOUR_VALUE;
	m_Ids["depth_value"] = ID_DEPTH_VALUE;
	m_Ids["stencil_value"] = ID_STENCIL_VALUE;

	m_Ids["check"] = ID_CHECK;
	m_Ids["comp_func"] = ID_COMP_FUNC;
	m_Ids["ref_value"] = ID_REF_VALUE;
	m_Ids["mask"] = ID_MASK;
	m_Ids["fail_op"] = ID_FAIL_OP;
	m_Ids["keep"] = ID_KEEP;
	m_Ids["increment"] = ID_INCREMENT;
	m_Ids["decrement"] = ID_DECREMENT;
	m_Ids["increment_wrap"] = ID_INCREMENT_WRAP;
	m_Ids["decrement_wrap"] = ID_DECREMENT_WRAP;
	m_Ids["invert"] = ID_INVERT;
	m_Ids["depth_fail_op"] = ID_DEPTH_FAIL_OP;
	m_Ids["pass_op"] = ID_PASS_OP;
	m_Ids["two_sided"] = ID_TWO_SIDED;
	m_Ids["read_back_as_texture"] = ID_READ_BACK_AS_TEXTURE;

	m_Ids["subroutine"] = ID_SUBROUTINE;

	m_Ids["line_width"] = ID_LINE_WIDTH;
	m_Ids["sampler"] = ID_SAMPLER;
	m_Ids["sampler_ref"] = ID_SAMPLER_REF;
	m_Ids["thread_groups"] = ID_THREAD_GROUPS;
	m_Ids["render_custom"] = ID_RENDER_CUSTOM;
	m_Ids["auto"] = ID_AUTO;
}

std::string getPropertyName(const ScriptCompiler* compiler, uint32_t id)
{
	for (const auto& kv : compiler->m_Ids)
	{
		if (kv.second == id)
			return kv.first;
	}
	ASSERT(false);
	return "unknown";
}

ScriptCompiler::AbstractTreeBuilder::AbstractTreeBuilder(ScriptCompiler* compiler)
	: mCompiler(compiler)
{
	mCurrent = nullptr;
}

void ScriptCompiler::AbstractTreeBuilder::visit(AbstractTreeBuilder* visitor, const std::list<ConcreteNode*>& nodes)
{
	for (auto node : nodes)
		visitor->visit(node);
}

void ScriptCompiler::AbstractTreeBuilder::visit(ConcreteNode* node)
{
	AbstractNode* asn = nullptr;

	if (node->type == CNT_IMPORT && !mCurrent)
	{
		ASSERT(false);
	}  
	else if (node->type == CNT_VARIABLE_ASSIGN)	  // variable set = "set" >> 2 children, children[0] == variable
	{
		ASSERT(false);
	}
	else if (node->type == CNT_VARIABLE)
	{
		if (!node->children.empty())
		{
			mCompiler->addError(CE_FEWERPARAMETERSEXPECTED, node->file.c_str(), node->line);
			return;
		}

		VariableAccessAbstractNode* impl = new VariableAccessAbstractNode(mCurrent);
		impl->line = node->line;
		impl->file = node->file;
		impl->name = node->token;

		asn = impl;
	}
	else if (!node->children.empty())			//properties and objects
	{
		ConcreteNode* temp1 = nullptr;
		ConcreteNode* temp2 = nullptr;
		auto riter = node->children.rbegin();

		if (riter != node->children.rend())
		{
			temp1 = *riter;
			++riter;
		}
		if (riter != node->children.rend())
		{
			temp2 = *riter;
		}

		// object = last 2 children == { and }
		if (temp1 && temp2 && temp1->type == CNT_RBRACE && temp2->type == CNT_LBRACE)
		{
			if (node->children.size() < 2)
			{
				mCompiler->addError(CE_STRINGEXPECTED, node->file.c_str(), node->line);
				return;
			}

			ObjectAbstractNode* impl = new ObjectAbstractNode(mCurrent);
			impl->line = node->line;
			impl->file = node->file;
			impl->abstract = false;

			std::list<ConcreteNode*> temp;
			if (node->token == "abstract")
			{
				impl->abstract = true;
				for (ConcreteNode* n : node->children)
				{
					temp.push_back(n);
				}
			}
			else
			{
				temp.push_back(node);
				for (ConcreteNode* n : node->children)
				{
					temp.push_back(n);
				}
			}

			//Get the type of object
			auto iter = temp.begin();
			impl->cls = (*iter)->token;
			++iter;

			//try to map the cls to an id
			auto iter2 = mCompiler->m_Ids.find(impl->cls);
			if (iter2 != mCompiler->m_Ids.end())
				impl->id = iter2->second;
			else
				mCompiler->addError(CE_UNEXPECTEDTOKEN, impl->file.c_str(), impl->line,
					std_string_format("'%s'. If this is a legacy script you must prepend the type (e.g. font, overlay).", impl->cls.c_str()).c_str());

			// Get the name
			// Unless the type is in the exclusion list
			if (iter != temp.end() && ((*iter)->type == CNT_WORD || (*iter)->type == CNT_QUOTE &&
				!mCompiler->isNameExcluded(*impl, mCurrent)))
			{
				impl->name = (*iter)->token;
				++iter;
			}

			// Everything up until the colon is a "value" of this object
			while (iter != temp.end() && (*iter)->type != CNT_COLON && (*iter)->type != CNT_LBRACE)
			{
				if ((*iter)->type == CNT_VARIABLE)
				{
					VariableAccessAbstractNode* var = new VariableAccessAbstractNode(impl);
					var->file = (*iter)->file;
					var->line = (*iter)->line;
					var->type = ANT_VARIABLE_ACCESS;
					var->name = (*iter)->token;
					impl->values.push_back(var);
				}
				else
				{
					AtomAbstractNode* atom = new AtomAbstractNode(impl);
					atom->file = (*iter)->file;
					atom->line = (*iter)->line;
					atom->type = ANT_ATOM;
					atom->value = (*iter)->token;

					auto idpos = mCompiler->m_Ids.find(atom->value);
					if (idpos != mCompiler->m_Ids.end())
						atom->id = idpos->second;

					impl->values.push_back(atom);
				}
				++iter;
			}

			//Find the bases
			if (iter != temp.end() && (*iter)->type == CNT_COLON)
			{
				for (ConcreteNode* n : (*iter)->children)
				{
					impl->bases.push_back(n->token);
				}
				++iter;
			}

			asn = impl;
			mCurrent = impl;

			// Visit the children of the {
			AbstractTreeBuilder::visit(this, temp2->children);

			mCurrent = impl->parent;
		}
		else  //it's a property
		{
			PropertyAbstractNode* impl = new PropertyAbstractNode(mCurrent);
			impl->line = node->line;
			impl->file = node->file;
			impl->name = node->token;

			auto iter2 = mCompiler->m_Ids.find(impl->name);
			if (iter2 != mCompiler->m_Ids.end())
				impl->id = iter2->second;

			asn = impl;
			mCurrent = impl;

			// Visit the children of the {
			AbstractTreeBuilder::visit(this, node->children);

			mCurrent = impl->parent;
		}
	}
	else            // standard atom
	{
		AtomAbstractNode* impl = new AtomAbstractNode(mCurrent);
		impl->line = node->line;
		impl->file = node->file;
		impl->value = node->token;

		auto iter2 = mCompiler->m_Ids.find(impl->value);
		if (iter2 != mCompiler->m_Ids.end())
			impl->id = iter2->second;

		asn = impl;
	}

	if (asn)
	{
		if (mCurrent)
		{
			if (mCurrent->type == ANT_PROPERTY)
			{
				PropertyAbstractNode* impl = static_cast<PropertyAbstractNode*>(mCurrent);
				impl->values.push_back(asn);
			}
			else
			{
				ObjectAbstractNode* impl = static_cast<ObjectAbstractNode*>(mCurrent);
				impl->children.push_back(asn);
			}
		}
		else
		{
			mNodes.push_back(asn);
		}
	}
}

ScriptCompilerManager::ScriptCompilerManager()
	: m_ScriptCompiler(this)
{
	addScriptPattern("*.material");
}

ScriptCompilerManager::~ScriptCompilerManager()
{

}

void ScriptCompilerManager::setListener(ScriptCompilerListener* listener)
{
	m_ScriptCompiler.setListener(listener);
}

ScriptCompilerListener* ScriptCompilerManager::getListener() const
{
	return m_ScriptCompiler.getListener();
}

ScriptTranslator* ScriptCompilerManager::getTranslator(const AbstractNode* node)
{
	ScriptTranslator* translator = nullptr;
	if (node->type == ANT_OBJECT)
	{
		const ObjectAbstractNode* obj = static_cast<const ObjectAbstractNode*>(node);
		const ObjectAbstractNode* parent = obj->parent ? static_cast<const ObjectAbstractNode*>(obj->parent) : nullptr;

		if (obj->id == ID_MATERIAL)
			translator = &m_MaterialTranslator;
		else if (obj->id == ID_TECHNIQUE && parent && parent->id == ID_MATERIAL)
			translator = &m_TechniqueTranslator;
		else if (obj->id == ID_PASS && parent && parent->id == ID_TECHNIQUE)
			translator = &m_PassTranslator;
		else if (obj->id == ID_TEXTURE_UNIT && parent && parent->id == ID_PASS)
			translator = &m_TextureUnitTranslator;
		else if (obj->id == ID_TEXTURE_SOURCE && parent && parent->id == ID_TEXTURE_UNIT)
			translator = &m_TextureSourceTranslator;
		else if (obj->id == ID_FRAGMENT_PROGRAM ||
			obj->id == ID_VERTEX_PROGRAM ||
			obj->id == ID_GEOMETRY_PROGRAM ||
			obj->id == ID_TESSELLATION_HULL_PROGRAM ||
			obj->id == ID_TESSELLATION_DOMAIN_PROGRAM ||
			obj->id == ID_COMPUTE_PROGRAM)
			translator = &m_GpuProgramTranslator;
	}
	return translator;
}

void ScriptCompilerManager::addScriptPattern(const char* pattern)
{
	m_ScriptPatterns.push_back(pattern);
}

bool ScriptCompilerManager::parseScript(const char* str, const char* source)
{
	std::string error;
	std::vector<ScriptToken> tokenList = ScriptLexer::tokenize(str, source, error);
	if (!error.empty())
		return false;

	std::list<ConcreteNode*> nodes = ScriptParser::parse(tokenList);
	return m_ScriptCompiler.compile(nodes);
}

ObjectAbstractNode::ObjectAbstractNode(AbstractNode* _parent)
	: AbstractNode(_parent), id (0), abstract(false)
{
	type = ANT_OBJECT;
}

void ObjectAbstractNode::addVariable(const char* name)
{
	mEnv[name] = "";
}

void ObjectAbstractNode::setVariable(const char* name, const char* value)
{
	mEnv[name] = value;
}

std::pair<bool, std::string> ObjectAbstractNode::getVariable(const char* name) const
{
	auto itr = mEnv.find(name);
	if (itr != mEnv.end())
		return std::make_pair(true, itr->second);

	ObjectAbstractNode* parentNode = static_cast<ObjectAbstractNode*>(parent);
	while (parentNode)
	{
		itr = parentNode->mEnv.find(name);
		if (itr != parentNode->mEnv.end())
			return std::make_pair(true, itr->second);
		parentNode = static_cast<ObjectAbstractNode*>(parentNode->parent);
	}
	return std::make_pair(false, "");
}

PropertyAbstractNode::PropertyAbstractNode(AbstractNode* _parent)
	: AbstractNode(_parent), id (0)
{
	type = ANT_PROPERTY;
}

ImportAbstractNode::ImportAbstractNode()
	: AbstractNode(nullptr)
{
	type = ANT_IMPORT;
}

VariableAccessAbstractNode::VariableAccessAbstractNode(AbstractNode* _parent)
	: AbstractNode(_parent)
{
	type = ANT_VARIABLE_ACCESS;
}

void ScriptCompilerListener::preConversion(ScriptCompiler* compiler, const std::list<ConcreteNode*>& nodes)
{

}

bool ScriptCompilerListener::postConversion(ScriptCompiler* compiler, const std::list<AbstractNode*>& nodes)
{
	return true;
}

void ScriptCompilerListener::handleError(ScriptCompiler* compiler, uint32_t code, const char* file, int line, const char* msg)
{
	std::string errMsg = std_string_format("ScriptCompiler - %s in %s (%d)",
		ScriptCompiler::formatErrorCode(code), file, line);
	if (strlen(msg) > 0)
	{
		errMsg += ": ";
		errMsg += msg;
	}

	printf("%s\n", errMsg.c_str());
}

bool ScriptCompilerListener::handleEvent(ScriptCompiler* compiler, ScriptCompilerEvent* evt, void* retval)
{
	return false;
}
