#include "pch.h"
#include "CGameObjectEx.h"

#include <Engine/CGameObject.h>
#include <Engine/CComponent.h>

CGameObjectEx::CGameObjectEx()
{
}

CGameObjectEx::~CGameObjectEx()
{
}
/*
* Engine Level���� Render ����� �������� �θ��� finaltick�� ���� ����
*/
void CGameObjectEx::finaltick()
{
	for (UINT i{ 0 }; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if(GetComponent((COMPONENT_TYPE)i))
			GetComponent((COMPONENT_TYPE)i)->finaltick();
	}

	vector<CGameObject*>::iterator iter = m_vecChild.begin();
	for (; iter != m_vecChild.end();) 
	{
		(*iter)->finaltick();

		if ((*iter)->IsDead())
		{
			iter = m_vecChild.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}
