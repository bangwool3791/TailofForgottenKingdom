#pragma once

#include "chunkgroup.h"
/*
* ���ø� Ŭ������ static ���� ���
*/
extern std::unordered_map<std::string, ChunkGroup*>			g_map_pool;
/*
* inline���� ���� ����
* inline�� ODR(One definion rull) �ذ� ����
* inline Ű���� �ٸ� �ߺ� ������ �����Ѵ�.
* #include �ǹ� �ڵ带 �ٿ��� �ְٴ�
*/
/*  �̰ͺ��� �� ���� �̱����� ���� �ִ�.
	��Ȳ�� �´� ���� �̱����� ����ϱ� �ٶ���. */
template <typename T, int MAX_OBJ_NUM, const char* pName>
ChunkGroup* ChunkGroupInstance()
{
	auto iter = g_map_pool.find(pName);

	if (iter == g_map_pool.end())
		g_map_pool[pName] = new ChunkGroup(sizeof(T), MAX_OBJ_NUM);

	return g_map_pool[pName];
}

/* ������θ� ����Ѵٸ�, ���ø����� �ھ� �д�. */
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
