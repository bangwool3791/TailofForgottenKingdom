#pragma once

#include <memory>

class MemPool
{
private:

	struct _Unit
	{
		struct _Unit* pPrev, * pNext;
	};

	//메모리 폴에 할당 된, 전체 메모리의 시작 주소
	void* m_pMemBlock;

	struct _Unit* m_pAllocatedMemBlock;
	struct _Unit* m_pFreeMemBlock;

	unsigned long m_ulUnitSize;
	unsigned long m_ulBlockSize;

public:
	MemPool(unsigned long ulUnitNum, unsigned long ulUnitSize)
		:m_pMemBlock{}
		, m_pAllocatedMemBlock{}
		, m_pFreeMemBlock{}
		, m_ulUnitSize(ulUnitSize)
		, m_ulBlockSize(ulUnitNum* (ulUnitSize + sizeof(_Unit)))
	{
		m_pMemBlock = malloc(m_ulBlockSize);

		if (m_pMemBlock)
		{
			for (unsigned long i{}; i < ulUnitNum; ++i)
			{
				struct _Unit* pCurUnit = (struct _Unit*)((char*)m_pMemBlock + i * (ulUnitSize + sizeof(_Unit)));

				pCurUnit->pPrev = nullptr;
				pCurUnit->pNext = m_pFreeMemBlock;

				if (nullptr != m_pFreeMemBlock)
				{
					m_pFreeMemBlock->pPrev = pCurUnit;
				}
				m_pFreeMemBlock = pCurUnit;
			}
		}
	}
	~MemPool()
	{
		free(m_pFreeMemBlock);
	}

	void* Alloc(unsigned long ulSize, bool bUseMempool = true)
	{
		if (ulSize > m_ulUnitSize || false == bUseMempool || nullptr == m_pMemBlock || nullptr == m_pFreeMemBlock)
		{
			return malloc(ulSize);
		}

		struct _Unit* pCurUnit = m_pFreeMemBlock;

		m_pFreeMemBlock = pCurUnit->pNext;

		if (nullptr != m_pFreeMemBlock)
		{
			//자유 메모리 영역에서 메모리 한 덩어리를 잘라낸다.
			m_pFreeMemBlock->pPrev = nullptr;
		}

		/*
		* 이전에 할당된 사용 메모리 블록 영역을 가리킨다.
		*/
		pCurUnit->pNext = m_pAllocatedMemBlock;

		if (nullptr != m_pAllocatedMemBlock)
		{
			//이전에 가리킨 사용 메모리 노드는
			//이전 값으로 현재의 노드를 가리킨다.
			m_pAllocatedMemBlock->pPrev = pCurUnit;
		}
		m_pAllocatedMemBlock = pCurUnit;

		return (void*)((char*)pCurUnit + sizeof(struct _Unit));
	}

	void Free(void* p)
	{
		if (m_pMemBlock < p && p < (void*)((char*)m_pMemBlock + m_ulBlockSize))
		{
			/*
			* 실제 할당 되는 메모리는 ((char*)pCurUnit + sizeof(struct _Unit))이므로
			* 이전, 다음 노드를 찾기 위해 sizeof(_Unit) 만큼 빼준다
			*/
			struct _Unit* pCurUnit = (_Unit*)((char*)p - sizeof(_Unit));
			
			struct _Unit* pPrev = pCurUnit->pPrev;
			struct _Unit* pNext = pCurUnit->pNext;

			/*
			* 삭제 노드가 맨 앞의 노드라면
			*/
			if (pPrev == nullptr)
			{
				/*
				* m_pAllocatedMemBlock 사용 노드의 맨 앞 노드를 가리킨다.
				*/
				m_pAllocatedMemBlock = pCurUnit->pNext;
			}
			/*
			* 삭제 노드가 중간 노드라면
			*/
			else
			{
				/*
				* 삭제 노드만 노드에서 뺀다
				*/
				pPrev->pNext = pNext;
			}
			
			/*
			* 현재 노드는 자유 노드의 맨 앞 노드로 이동
			*/
			pCurUnit->pNext = m_pFreeMemBlock;
			/*
			* 자유 노드가 할당 되었으면
			*/
			if (nullptr != m_pFreeMemBlock)
			{
				m_pFreeMemBlock->pPrev = pCurUnit;
			}

			m_pFreeMemBlock = pCurUnit;
		}
		else
		{
			free(p);
		}
	}

};