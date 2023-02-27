#pragma once

#include "chunkgroup.h"
/*
* 템플릿 클래스의 static 변수 사용
*/
extern std::unordered_map<std::string, ChunkGroup*>			g_map_pool;
/*
* inline으로 선언 가능
* inline시 ODR(One definion rull) 해결 가능
* inline 키워드 다른 중복 선언을 무시한다.
* #include 의미 코드를 붙여서 넣겟다
*/
/*  이것보다 더 좋은 싱글턴은 많이 있다.
	상황에 맞는 좋은 싱글턴을 사용하길 바란다. */
template <typename T, int MAX_OBJ_NUM, const char* pName>
ChunkGroup* ChunkGroupInstance()
{
	auto iter = g_map_pool.find(pName);

	if (iter == g_map_pool.end())
		g_map_pool[pName] = new ChunkGroup(sizeof(T), MAX_OBJ_NUM);

	return g_map_pool[pName];
}

/* 상속으로만 사용한다면, 템플릿으로 박아 둔다. */
template <typename T, size_t MAX_OBJ_NUM, const char* pName>
class SmallObjAllocator
{
public:
	static void* operator new(size_t s)
	{
		return ChunkGroupInstance<T, MAX_OBJ_NUM, pName>()->alloc();
	}

	static void operator delete(void* pdelloc_target, std::size_t obj_size)
	{
		if (NULL == pdelloc_target)
			return;

		ChunkGroupInstance<T, MAX_OBJ_NUM, pName>()->delloc(pdelloc_target);
	}
};
