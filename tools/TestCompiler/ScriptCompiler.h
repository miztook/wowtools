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
	std::list<AbstractNode*>	overrides;

public:
	ObjectAbstractNode(AbstractNode* _parent);
	const char* getValue() const override { return cls.c_str(); }

	void addVariable(const char* name);
	void setVariable(const char* name, const char* value);
	std::pair<bool, std::string> getVariable(const char* name) const;
	const std::map<std::string, std::string>& getVariables() const { return mEnv; }

};

class PropertyAbstractNode : public AbstractNode
{
public: 
	std::string name;
	uint32_t id;
public:
	PropertyAbstractNode(AbstractNode* _parent);
	const char* getValue() const override { return name.c_str(); }
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
	virtual ~ScriptCompilerListener() {}

public:
	virtual void preConversion(ScriptCompiler* compiler, const std::list<ConcreteNode*>& nodes);
	virtual bool postConversion(ScriptCompiler* compiler, const std::list<AbstractNode*>& nodes);
	virtual void handleError(ScriptCompiler* compiler, uint32_t code, const char* file, int line, const char* msg);
	virtual bool handleEvent(ScriptCompiler* compiler, ScriptCompilerEvent* evt, void* retval);
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
	void setListener(ScriptCompilerListener* listener) { m_Listener = listener; }
	ScriptCompilerListener* getListener() const { return m_Listener; }
	bool _fireEvent(ScriptCompilerEvent* evt, void* retVal);

	ScriptCompilerManager* getScriptCompilerManager() const { return m_scriptCompilerManager; }

	const std::list<Error>& getErrorList() const { return m_Errors; }

private:
	std::list<AbstractNode*> convertToAST(const std::list<ConcreteNode*>& nodes);

	void processImports(std::list<AbstractNode*>& nodes);

	std::list<AbstractNode*> locateTarget(const std::list<AbstractNode*>& nodes, const char* target);

	void processObjects(std::list<AbstractNode*>& nodes, const std::list<AbstractNode*>& top);

	void processVariables(std::list<AbstractNode*>& nodes);

	bool isNameExcluded(const ObjectAbstractNode& node, const AbstractNode* parent);

	void initWordMap();

private:
	friend std::string getPropertyName(const ScriptCompiler* compiler, uint32_t id);

	std::map<std::string, uint32_t> m_Ids;
	std::map<std::string, std::string> m_Env;

	std::list<Error> m_Errors;

	ScriptCompilerListener* m_Listener;
	ScriptCompilerManager* m_scriptCompilerManager;
	ScriptCompilerListener m_defaultListener;
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

class ScriptTranslator;

class ScriptCompilerManager
{
private:
	std::vector<std::string>	m_ScriptPatterns;

	ScriptCompiler	m_ScriptCompiler;

public:
	ScriptCompilerManager();
	virtual ~ScriptCompilerManager();

	void setListener(ScriptCompilerListener* listener);
	ScriptCompilerListener* getListener() const;

	ScriptTranslator* getTranslator(const AbstractNode* node);

	void addScriptPattern(const char* pattern);
	const std::vector<std::string>& getScriptPatterns() const { return m_ScriptPatterns; }

	bool parseScript(const char* str, const char* source);

private:
	MaterialTranslator	m_MaterialTranslator;
	TechniqueTranslator		m_TechniqueTranslator;
	PassTranslator	m_PassTranslator;
	TextureUnitTranslator	m_TextureUnitTranslator;
	SamplerTranslator	m_SamplerTranslator;
	TextureSourceTranslator		m_TextureSourceTranslator;
	GpuProgramTranslator	m_GpuProgramTranslator;
};

class CreateMaterialScriptCompilerEvent : public ScriptCompilerEvent
{
public:
	std::string mFile;
	std::string mName;

	CreateMaterialScriptCompilerEvent(const char* file, const char* name)
		: ScriptCompilerEvent("createMaterial"), mFile(file), mName(name) {}
};

enum
{
	ID_MATERIAL = 3,
	ID_VERTEX_PROGRAM,
	ID_GEOMETRY_PROGRAM,
	ID_FRAGMENT_PROGRAM,
	ID_TECHNIQUE,
	ID_PASS,
	ID_TEXTURE_UNIT,
	ID_VERTEX_PROGRAM_REF,
	ID_GEOMETRY_PROGRAM_REF,
	ID_FRAGMENT_PROGRAM_REF,
	ID_SHADOW_CASTER_VERTEX_PROGRAM_REF,
	ID_SHADOW_CASTER_FRAGMENT_PROGRAM_REF,
	ID_SHADOW_RECEIVER_VERTEX_PROGRAM_REF,
	ID_SHADOW_RECEIVER_FRAGMENT_PROGRAM_REF,
	ID_SHADOW_CASTER_MATERIAL,
	ID_SHADOW_RECEIVER_MATERIAL,

	ID_LOD_VALUES,
	ID_LOD_STRATEGY,
	ID_LOD_DISTANCES,
	ID_RECEIVE_SHADOWS,
	ID_TRANSPARENCY_CASTS_SHADOWS,
	ID_SET_TEXTURE_ALIAS,

	ID_SOURCE,
	ID_SYNTAX,
	ID_DEFAULT_PARAMS,
	ID_PARAM_INDEXED,
	ID_PARAM_NAMED,
	ID_PARAM_INDEXED_AUTO,
	ID_PARAM_NAMED_AUTO,

	ID_SCHEME,
	ID_LOD_INDEX,
	ID_GPU_VENDOR_RULE,
	ID_GPU_DEVICE_RULE,
	ID_INCLUDE,
	ID_EXCLUDE,

	ID_AMBIENT,
	ID_DIFFUSE,
	ID_SPECULAR,
	ID_EMISSIVE,
	ID_VERTEXCOLOUR,
	ID_SCENE_BLEND,
	ID_COLOUR_BLEND,
	ID_ONE,
	ID_ZERO,
	ID_DEST_COLOUR,
	ID_SRC_COLOUR,
	ID_ONE_MINUS_DEST_COLOUR,
	ID_ONE_MINUS_SRC_COLOUR,
	ID_DEST_ALPHA,
	ID_SRC_ALPHA,
	ID_ONE_MINUS_DEST_ALPHA,
	ID_ONE_MINUS_SRC_ALPHA,
	ID_SEPARATE_SCENE_BLEND,
	ID_SCENE_BLEND_OP,
	ID_REVERSE_SUBTRACT,
	ID_MIN,
	ID_MAX,
	ID_SEPARATE_SCENE_BLEND_OP,
	ID_DEPTH_CHECK,
	ID_DEPTH_WRITE,
	ID_DEPTH_FUNC,
	ID_DEPTH_BIAS,
	ID_ITERATION_DEPTH_BIAS,
	ID_ALWAYS_FAIL,
	ID_ALWAYS_PASS,
	ID_LESS_EQUAL,
	ID_LESS,
	ID_EQUAL,
	ID_NOT_EQUAL,
	ID_GREATER_EQUAL,
	ID_GREATER,
	ID_ALPHA_REJECTION,
	ID_ALPHA_TO_COVERAGE,
	ID_LIGHT_SCISSOR,
	ID_LIGHT_CLIP_PLANES,
	ID_TRANSPARENT_SORTING,
	ID_ILLUMINATION_STAGE,
	ID_DECAL,
	ID_CULL_HARDWARE,
	ID_CLOCKWISE,
	ID_ANTICLOCKWISE,
	ID_CULL_SOFTWARE,
	ID_BACK,
	ID_FRONT,
	ID_NORMALISE_NORMALS,
	ID_LIGHTING,
	ID_SHADING,
	ID_FLAT,
	ID_GOURAUD,
	ID_PHONG,
	ID_POLYGON_MODE,
	ID_SOLID,
	ID_WIREFRAME,
	ID_POINTS,
	ID_POLYGON_MODE_OVERRIDEABLE,
	ID_FOG_OVERRIDE,
	ID_NONE,
	ID_LINEAR,
	ID_EXP,
	ID_EXP2,
	ID_COLOUR_WRITE,
	ID_MAX_LIGHTS,
	ID_START_LIGHT,
	ID_ITERATION,
	ID_ONCE,
	ID_ONCE_PER_LIGHT,
	ID_PER_LIGHT,
	ID_PER_N_LIGHTS,
	ID_POINT,
	ID_SPOT,
	ID_DIRECTIONAL,
	ID_LIGHT_MASK,
	ID_POINT_SIZE,
	ID_POINT_SPRITES,
	ID_POINT_SIZE_ATTENUATION,
	ID_POINT_SIZE_MIN,
	ID_POINT_SIZE_MAX,

	ID_TEXTURE_ALIAS,
	ID_TEXTURE,
	ID_1D,
	ID_2D,
	ID_3D,
	ID_CUBIC,
	ID_2DARRAY,
	ID_UNLIMITED,
	ID_ALPHA,
	ID_GAMMA,
	ID_ANIM_TEXTURE,
	ID_CUBIC_TEXTURE,
	ID_SEPARATE_UV,
	ID_COMBINED_UVW,
	ID_TEX_COORD_SET,
	ID_TEX_ADDRESS_MODE,
	ID_WRAP,
	ID_CLAMP,
	ID_BORDER,
	ID_MIRROR,
	ID_TEX_BORDER_COLOUR,
	ID_FILTERING,
	ID_BILINEAR,
	ID_TRILINEAR,
	ID_ANISOTROPIC,
	ID_CMPTEST,
	ID_ON,
	ID_OFF,
	ID_CMPFUNC,
	ID_MAX_ANISOTROPY,
	ID_MIPMAP_BIAS,
	ID_COLOUR_OP,
	ID_REPLACE,
	ID_ADD,
	ID_MODULATE,
	ID_ALPHA_BLEND,
	ID_COLOUR_OP_EX,
	ID_SOURCE1,
	ID_SOURCE2,
	ID_MODULATE_X2,
	ID_MODULATE_X4,
	ID_ADD_SIGNED,
	ID_ADD_SMOOTH,
	ID_SUBTRACT,
	ID_BLEND_DIFFUSE_COLOUR,
	ID_BLEND_DIFFUSE_ALPHA,
	ID_BLEND_TEXTURE_ALPHA,
	ID_BLEND_CURRENT_ALPHA,
	ID_BLEND_MANUAL,
	ID_DOT_PRODUCT,
	ID_SRC_CURRENT,
	ID_SRC_TEXTURE,
	ID_SRC_DIFFUSE,
	ID_SRC_SPECULAR,
	ID_SRC_MANUAL,
	ID_COLOUR_OP_MULTIPASS_FALLBACK,
	ID_ALPHA_OP_EX,
	ID_ENV_MAP,
	ID_SPHERICAL,
	ID_PLANAR,
	ID_CUBIC_REFLECTION,
	ID_CUBIC_NORMAL,
	ID_SCROLL,
	ID_SCROLL_ANIM,
	ID_ROTATE,
	ID_ROTATE_ANIM,
	ID_SCALE,
	ID_WAVE_XFORM,
	ID_SCROLL_X,
	ID_SCROLL_Y,
	ID_SCALE_X,
	ID_SCALE_Y,
	ID_SINE,
	ID_TRIANGLE,
	ID_SQUARE,
	ID_SAWTOOTH,
	ID_INVERSE_SAWTOOTH,
	ID_TRANSFORM,
	ID_BINDING_TYPE,
	ID_VERTEX,
	ID_FRAGMENT,
	ID_CONTENT_TYPE,
	ID_NAMED,
	ID_SHADOW,
	ID_TEXTURE_SOURCE,
	ID_SHARED_PARAMS,
	ID_SHARED_PARAM_NAMED,
	ID_SHARED_PARAMS_REF,

	ID_PARTICLE_SYSTEM,
	ID_EMITTER,
	ID_AFFECTOR,

	ID_COMPOSITOR,
	ID_TARGET,
	ID_TARGET_OUTPUT,

	ID_INPUT,
	ID_PREVIOUS,
	ID_TARGET_WIDTH,
	ID_TARGET_HEIGHT,
	ID_TARGET_WIDTH_SCALED,
	ID_TARGET_HEIGHT_SCALED,
	ID_COMPOSITOR_LOGIC,
	ID_TEXTURE_REF,
	ID_SCOPE_LOCAL,
	ID_SCOPE_CHAIN,
	ID_SCOPE_GLOBAL,
	ID_POOLED,
	//ID_GAMMA, - already registered for material
	ID_NO_FSAA,
	ID_DEPTH_POOL,
	ID_ONLY_INITIAL,
	ID_VISIBILITY_MASK,
	ID_LOD_BIAS,
	ID_MATERIAL_SCHEME,
	ID_SHADOWS_ENABLED,

	ID_CLEAR,
	ID_STENCIL,
	ID_RENDER_SCENE,
	ID_RENDER_QUAD,
	ID_IDENTIFIER,
	ID_FIRST_RENDER_QUEUE,
	ID_LAST_RENDER_QUEUE,
	ID_QUAD_NORMALS,
	ID_CAMERA_FAR_CORNERS_VIEW_SPACE,
	ID_CAMERA_FAR_CORNERS_WORLD_SPACE,

	ID_BUFFERS,
	ID_COLOUR,
	ID_DEPTH,
	ID_COLOUR_VALUE,
	ID_DEPTH_VALUE,
	ID_STENCIL_VALUE,

	ID_CHECK,
	ID_COMP_FUNC,
	ID_REF_VALUE,
	ID_MASK,
	ID_FAIL_OP,
	ID_KEEP,
	ID_INCREMENT,
	ID_DECREMENT,
	ID_INCREMENT_WRAP,
	ID_DECREMENT_WRAP,
	ID_INVERT,
	ID_DEPTH_FAIL_OP,
	ID_PASS_OP,
	ID_TWO_SIDED,
	ID_READ_BACK_AS_TEXTURE,
	// Suport for shader model 5.0
	// More program IDs
	ID_TESSELLATION_HULL_PROGRAM,
	ID_TESSELLATION_DOMAIN_PROGRAM,
	ID_COMPUTE_PROGRAM,
	ID_TESSELLATION_HULL_PROGRAM_REF,
	ID_TESSELLATION_DOMAIN_PROGRAM_REF,
	ID_COMPUTE_PROGRAM_REF,
	// More binding IDs
	ID_GEOMETRY,
	ID_TESSELLATION_HULL,
	ID_TESSELLATION_DOMAIN,
	ID_COMPUTE,

	// Support for subroutine
	ID_SUBROUTINE,

	// added during 1.11. re-sort for 1.12
	ID_LINE_WIDTH,
	ID_SAMPLER,
	ID_SAMPLER_REF,
	ID_THREAD_GROUPS,
	ID_RENDER_CUSTOM,
	ID_AUTO,

	ID_END_BUILTIN_IDS
};