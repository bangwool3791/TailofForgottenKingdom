#include "pch.h"
#include "smallobjallocator.h"

//#include "CTransform.h"
//#include "CCollider2D.h"
//#include "CMeshRender.h"
//#include "CRenderComponent.h"
//#include "CAnimation2D.h"
//#include "CAnimator2D.h"
//#include "CGameObject.h"
//
//template <typename T, size_t MAX_OBJ_NUM>
//class SmallObjAllocator
//{
//public:
//	static void* operator new(size_t s)
//	{
//		return ChunkGroupInstance<T, MAX_OBJ_NUM>()->alloc();
//	}
//
//	static void operator delete(void* pdelloc_target, std::size_t obj_size)
//	{
//		if (NULL == pdelloc_target)
//			return;
//
//		ChunkGroupInstance<T, MAX_OBJ_NUM>()->delloc(pdelloc_target);
//	}
//};
//
//
//template <typename T, int MAX_OBJ_NUM>
//ChunkGroup* ChunkGroupInstance()
//{
//	string strkey{};
//
//	if (typeid(T).hash_code() == typeid(CTransform).hash_code())
//		strkey = "CTransform";
//
//	if (typeid(T).hash_code() == typeid(CCollider2D).hash_code())
//		strkey = "CCollider2D";
//
//	if (typeid(T).hash_code() == typeid(CMeshRender).hash_code())
//		strkey = "CMeshRender";
//
//	if (typeid(T).hash_code() == typeid(CRenderComponent).hash_code())
//		strkey = "CRenderComponent";
//
//	if (typeid(T).hash_code() == typeid(CAnimation2D).hash_code())
//		strkey = "CAnimation2D";
//
//	if (typeid(T).hash_code() == typeid(CAnimator2D).hash_code())
//		strkey = "CAnimator2D";
//
//	if (typeid(T).hash_code() == typeid(CGameObject).hash_code())
//		strkey = "CGameObject";
//	
//	assert(!strkey.empty());
//
//	auto iter = g_map_pool.find(strkey);
//
//	if (iter == g_map_pool.end())
//		g_map_pool[strkey] = new ChunkGroup(sizeof(T), MAX_OBJ_NUM);
//
//	return g_map_pool[strkey];
//}