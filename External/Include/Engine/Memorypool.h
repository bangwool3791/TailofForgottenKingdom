#pragma once

#include <memory>

class MemPool
{
private:

	struct _Unit
	{
		struct _Unit* pPrev, * pNext;
	};

	//�޸� ���� �Ҵ� ��, ��ü �޸��� ���� �ּ�
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
			//���� �޸� �������� �޸� �� ����� �߶󳽴�.
			m_pFreeMemBlock->pPrev = nullptr;
		}

		/*
		* ������ �Ҵ�� ��� �޸� ��� ������ ����Ų��.
		*/
		pCurUnit->pNext = m_pAllocatedMemBlock;

		if (nullptr != m_pAllocatedMemBlock)
		{
			//������ ����Ų ��� �޸� ����
			//���� ������ ������ ��带 ����Ų��.
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
			* ���� �Ҵ� �Ǵ� �޸𸮴� ((char*)pCurUnit + sizeof(struct _Unit))�̹Ƿ�
			* ����, ���� ��带 ã�� ���� sizeof(_Unit) ��ŭ ���ش�
			*/
			struct _Unit* pCurUnit = (_Unit*)((char*)p - sizeof(_Unit));
			
			struct _Unit* pPrev = pCurUnit->pPrev;
			struct _Unit* pNext = pCurUnit->pNext;

			/*
			* ���� ��尡 �� ���� �����
			*/
			if (pPrev == nullptr)
			{
				/*
				* m_pAllocatedMemBlock ��� ����� �� �� ��带 ����Ų��.
				*/
				m_pAllocatedMemBlock = pCurUnit->pNext;
			}
			/*
			* ���� ��尡 �߰� �����
			*/
			else
			{
				/*
				* ���� ��常 ��忡�� ����
				*/
				pPrev->pNext = pNext;
			}
			
			/*
			* ���� ���� ���� ����� �� �� ���� �̵�
			*/
			pCurUnit->pNext = m_pFreeMemBlock;
			/*
			* ���� ��尡 �Ҵ� �Ǿ�����
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