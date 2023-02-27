#include "pch.h"
#include "global.h"

tTransform									g_transform = {};
tGlobalData									g_global = {};
unordered_map<string, ChunkGroup*>			g_map_pool;
vector<tObjectRender>						g_vecInfoObject;
wchar_t g_szHp[255]{};
wchar_t g_szFullName[255]{};

const Vector3 Vector3::Zero{ 0.f, 0.f, 0.f };
const Vector3 Vector3::One{ 1.f, 1.f, 1.f };
const Vector3 Vector3::UnitX{ 1.f, 0.f, 0.f };
const Vector3 Vector3::UnitY{ 0.f, 1.f, 0.f };
const Vector3 Vector3::UnitZ{ 0.f, 0.f, 1.f };
const Vector3 Vector3::Up{ 0.f, 1.f, 0.f };
const Vector3 Vector3::Down{ 0.f, -1.f, 0.f };
const Vector3 Vector3::Right{ 1.f, 0.f, 0.f };
const Vector3 Vector3::Left{ -1.f, 0.f, 0.f };
const Vector3 Vector3::Forward;
const Vector3 Vector3::Front{ 0.f, 0.f, -1.f };
const Vector3 Vector3::Backward{ 0.f, 0.f, 1.f };

extern const char* COMPONENT_TYPE_CHAR[(UINT)COMPONENT_TYPE::END] =
{
	"TRANSFORM",
	"CAMERA",
	"COLLIDER2D",
	"COLLIDER3D",
	"ANIMATOR2D",
	"ANIMATOR3D",
	"LIGHT2D",
	"LIGHT3D",
	"MESHRENDER",
	"TILEMAP",
	"PARTICLESYSTEM",
	"SKYBOX",
	"DECAL",
	"LANDSCAPE",
	"SCRIPT"
};

extern const wchar_t* COMPONENT_TYPE_WCHAR[(UINT)COMPONENT_TYPE::END] =
{
	L"TRANSFORM",
	L"CAMERA",
	L"COLLIDER2D",
	L"COLLIDER3D",
	L"ANIMATOR2D",
	L"ANIMATOR3D",
	L"LIGHT2D",
	L"LIGHT3D",
	L"MESHRENDER",
	L"TILEMAP",
	L"PARTICLESYSTEM",
	L"EXPOLOSION",
	L"SKYBOX",
	L"DECAL",
	L"LANDSCAPE",
	L"SCRIPT"
};

extern const char* OBJECT_TYPE_CHAR[(UINT)OBJECT_TYPE::END] =
{
	"LIGHT2D",
	"LIGHT3D",
};

extern const wchar_t* OBJECT_TYPE_WCHAR[(UINT)OBJECT_TYPE::END] =
{
	L"LIGHT2D",
	L"LIGHT3D",
};


extern const char* RES_TYPE_CHAR[(UINT)RES_TYPE::END] =
{
	"PREFAB",
	"MESHDATA",
	"COMPUTE_SHADER",
	"MATERIAL",
	"MESH",
	"TEXTURE",
	"SOUND",
	"GRAPHICS_SHADER",
};

extern const wchar_t* RES_TYPE_WCHAR[(UINT)RES_TYPE::END] =
{
	L"PREFAB",
	L"MESHDATA",
	L"COMPUTE_SHADER",
	L"MATERIAL",
	L"MESH",
	L"TEXTURE",
	L"SOUND",
	L"GRAPHICS_SHADER",
};