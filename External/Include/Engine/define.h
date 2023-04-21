#pragma once

typedef Vector2 Vec2;
typedef Vector3 Vec3;
typedef Vector4 Vec4;

#define SINGLE(type) public:\
						static type* GetInst()\
						{\
							static type mgr;\
							return &mgr;\
						}\
						private:\
							type();\
							~type();

#define DT CTimeMgr::GetInst()->GetDeltaTime()

#define DEVICE CDevice::GetInst()->GetDevice()
#define CONTEXT CDevice::GetInst()->GetContext()

#define KEY_PRESSED(key)	CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::PRESS
#define KEY_TAP(key)		CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::TAP
#define KEY_RELEASE(key)	CKeyMgr::GetInst()->GetKeyState(key) == KEY_STATE::RELEASE
#define KEY_DIR	            CKeyMgr::GetInst()->GetMouseDir(key)

#define CLONE(type) public: type* Clone() { return new type(*this); }
#define CLONE_DUMMY(type) public : type* Clone(){return nullptr;}
#define CLONE_ASSERT(type) public : type* Clone(){assert(false); return nullptr;}

#define MAX_LAYER 32

#define ALPHABLEND_Z_POS 0.9f

#define TERRAINX 100
#define TERRAINZ 100

#define TERRAINCX 64 * 4.f
#define TERRAINCZ 64 * 4.f

#define TILEX 200
#define TILEZ 200

#define	TILECX 64 * 2.f
#define	TILECZ 64 * 2.f

#define OBJECTPOOL_SIZE 1000

enum class SHADER_DOMAIN
{
	DOMAIN_DEFERRED_OPAQUE,		// 지연 렌더링
	DOMAIN_DEFERRED_MASK,		// 지연 렌더링
	DOMAIN_DEFERRED_DECAL,		// 지연 렌더링

	DOMAIN_OPAQUE,		 // 불투명 단계
	DOMAIN_MASK,		 // 출력, 비출력
	DOMAIN_DECAL,		// 데칼 처리

	DOMAIN_TRANSPARENT,  // 반투명
	DOMAIN_POST_PROCESS, // 후처리	    
	DOMAIN_UI,			 // UI	  
	// Engine 차원에서 처리 
	DOMAIN_LIGHT,				// 광원 처리
	NONE,
};

enum class SAMPLER_TYPE
{
	ANISOTROPIC, // 이방성
	POINT,
	BLUR,
	SHADOW,
	END,
};

enum class CB_TYPE
{
	TRANSFORM,	 // b0
	MATERIAL,	 // b1
	GLOBAL,	     // b2
	ANIMATION2D, // b3
	END,
};

// Rasterizer State
enum class RS_TYPE
{
	CULL_BACK,
	CULL_FRONT,
	CULL_NONE,
	WIRE_FRAME,
	SHADOW,
	END,
};

// BlendState Type
enum class BS_TYPE
{
	DEFAULT,
	ALPHABLEND,
	ONE_ONE,

	END,
};

enum class DS_TYPE
{
	LESS,
	LESS_EQUAL,
	GREATER,
	NO_WRITE,
	NO_TEST_NO_WRITE,
	END,
};
enum PIPELINE_STAGE
{
	VS = 0x01,
	HS = 0x02,
	DS = 0x04,
	GS = 0x08,
	PS = 0x10,
	ALL_STAGE = VS | HS | DS | GS | PS,
};

enum class RES_TYPE
{
	PREFAB,
	MESHDATA,
	COMPUTE_SHADER,

	MATERIAL,
	MESH,
	TEXTURE,
	SOUND,
	GRAPHICS_SHADER,
	END,
};

enum class COMPONENT_TYPE
{
	TRANSFORM,
	MESHRENDER,
	CAMERA,
	COLLIDER2D,
	COLLIDER3D,
	ANIMATOR2D,
	ANIMATOR3D,
	LIGHT2D,
	LIGHT3D,
	TILEMAP,
	PARTICLESYSTEM,
	SKYBOX,
	DECAL,
	LANDSCAPE,
	WAVERENDERER,
	PHYSX,
	SCRIPT,
	END,
};

enum class OBJECT_TYPE
{
	LIGHT2D,
	LIGHT3D,
	END,
};

enum SCALAR_PARAM
{
	INT_0,
	INT_1,
	INT_2,
	INT_3,

	FLOAT_0,
	FLOAT_1,
	FLOAT_2,
	FLOAT_3,

	VEC2_0,
	VEC2_1,
	VEC2_2,
	VEC2_3,

	VEC4_0,
	VEC4_1,
	VEC4_2,
	VEC4_3,

	MAT_0,
	MAT_1,
	MAT_2,
	MAT_3,
};

enum TEX_PARAM
{
	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,

	TEX_4,
	TEX_5,
	TEX_6,
	TEX_7,

	TEX_8,
	TEX_9,
	TEX_10,
	TEX_11,

	TEX_12,
	TEX_13,
	TEX_14,
	TEX_15,

	TEX_16,
	TEX_17,
	TEX_18,
	TEX_19,

	TEX_20,
	TEX_21,
	TEX_22,
	TEX_23,

	TEX_24,
	TEX_25,
	TEX_26,
	TEX_27,

	TEX_28,
	TEX_29,
	TEX_30,
	TEX_31,

	TEX_32,
	TEX_CUBE_0,
	TEX_CUBE_1,
	TEX_ARR_0,

	TEX_ARR_1,
	TEX_PADDING1,
	TEX_PADDING2,
	TEX_PADDING3,

	TEX_END,
};

enum class DIR
{
	RIGHT,
	UP,
	FRONT,
	END,
};

enum class EVENT_TYPE
{
	CREATE_OBJECT,  // wParam : Object Adress, lParam : Layer Index
	DELETE_OBJECT,	// wParam : Object Adress,
	ADD_CHILD,		// wParam : Child Adress, lParam : Parent Adress
	ADD_RES,
	EDIT_RES,
	DELETE_RES,		// wParam : RES_TYPE, lParam : Resource Adress
	CHANGE_LEVEL,
	CHANGE_LEVEL_STATE,	// wParam : Next Level State
	END,
};

enum class DEBUG_SHAPE
{
	RECT,
	CIRCLE,

	CUBE,
	SPHERE,
	CONE,
	FRUSTUM,
	END,
};

enum class COLLIDER2D_TYPE
{
	COLLIDER2D_RECT,
	COLLIDER2D_CIRCLE,
};

enum class COLLIDER3D_TYPE
{
	COLLIDER3D_CUBE,
	COLLIDER3D_SPHERE,
};


enum class SB_TYPE
{
	SRV_ONLY,
	UAV_INC,
};

enum class LIGHT_TYPE
{
	DIRECTIONAL,
	POINT,
	POINT2,
	SPOT,
	SPOT2,
};
enum class UI_TYPE
{
	GAMEOBJECT,
	DRAG,
	UNIT_UI,
	ANIMATION_BOX,
	END,
};

enum class INSTANCING_TYPE
{
	NONE,
	USED,
};

enum class LEVEL_STATE
{
	PLAY,
	STOP,
	PAUSE,
	//LOADER,
};

enum TAP_CATEGORY
{
	COMMAND_CENTER,
	COLONISTS,
	RESOURCE,
	INDUSTRY,
	DEFENSE,
	SOLDIER_CMD,
	UNIT,
	TAP_CATEGORY_UPGRADE,
	TAP_CATEGORY_COMPLETE,
	TAP_CATEGORY_END,
};

enum class BUILD_STATE
{
	READY,
	BUILD,
	COMPLETE,
	DESTROY,
	CREATE_UNIT,
	END,
};

enum class TILE_TYPE
{
	NOTUSED,
	EMPTY,
	BUILD,
	COLLISION,
	USED,
	NOT_FOREST,
	BEFORE_HUNTED,
	HUNTED,
	CRYSTAL,
	BEFORE_CRYSTAL,
	WOOD,
	BEFROE_WOOD,
	HARVEST,
};

enum class UNIT_STATE
{
	NORMAL,
	RUN,
	ATTACK,
	LOADATTACK,
	DEAD,
};

enum class MRT_TYPE
{
	SWAPCHAIN,
	DEFERRED,
	DECAL,
	LIGHT,
	HDR,
	BLUR,
	BLOOM,
	SHADOW,
	FRONT,
	BACK,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	END,
};

enum class LANDSCAPE_MOD
{
	HEIGHT_MAP,
	SPLAT,
	NONE,
	SPLAT_CLEAR,
};

enum PhysType
{
	UNUSED = 0,
	PISTOL,
	ARTILLERY,
	FIREBALL,
	LASER
};

extern const char* COMPONENT_TYPE_CHAR[(UINT)COMPONENT_TYPE::END];
extern const wchar_t* COMPONENT_TYPE_WCHAR[(UINT)COMPONENT_TYPE::END];

extern const char* OBJECT_TYPE_CHAR[(UINT)OBJECT_TYPE::END];
extern const wchar_t* OBJECT_TYPE_WCHAR[(UINT)OBJECT_TYPE::END];

extern const char* RES_TYPE_CHAR[(UINT)RES_TYPE::END];
extern const wchar_t* RES_TYPE_WCHAR[(UINT)RES_TYPE::END];

class CEngine;
typedef void (CEngine::* PRINT_FUNC)(void);

#define SAFE_DELETE(p) if(nullptr != p) delete p; p = nullptr;
