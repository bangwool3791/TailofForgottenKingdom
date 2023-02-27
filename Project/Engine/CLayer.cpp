#include "pch.h"
#include "CLayer.h"

#include "CGameObject.h"

CLayer::CLayer()
{
}

CLayer::~CLayer()
{
	Safe_Del_Vec(m_vecParent);
}

void CLayer::begin()
{
	for (size_t i = 0; i < m_vecParent.size(); ++i)
	{
		m_vecParent[i]->begin();
	}
}

void CLayer::tick()
{
	for (auto iter{ m_vecParent.begin()}; iter != m_vecParent.end(); ++iter)
	{
		(*iter)->tick();
	}
}

void CLayer::finaltick()
{
	for (auto iter{ m_vecParent.begin() }; iter != m_vecParent.end();)
	{
		(*iter)->finaltick();

		if ((*iter)->IsDead())
		{
			iter = m_vecParent.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CLayer::render()
{
	for (auto iter{ m_vecParent.begin() }; iter != m_vecParent.end(); ++iter)
	{
		(*iter)->render();
	}
}

void CLayer::AddGameObject(CGameObject*_pObject)
{
	m_vecParent.push_back(_pObject);

	static queue<CGameObject*> que;

	que.push(_pObject);

	while (!que.empty())
	{
		auto pGameObecjt = que.front();
		que.pop();

		auto pVecChilds = pGameObecjt->GetChilds();

		for (auto elem{ pVecChilds.begin() }; elem != pVecChilds.end(); ++elem)
		{
			que.push(*elem);
		}

		pGameObecjt->m_iLayerIdx = m_iLayerIdx;
	}
}

void CLayer::DeregisterObject(CGameObject* _pObj)
{
	vector<CGameObject*>::iterator iter = m_vecParent.begin();

	for (; iter != m_vecParent.end(); ++iter)
	{
		if ((*iter) == _pObj)
		{
			m_vecParent.erase(iter);
			return;
		}
	}
	assert(nullptr);
}


CGameObject* CLayer::FindParent(const wstring& _name)
{

	auto iter{ m_vecParent.cbegin() };

	for (; iter != m_vecParent.cend(); ++iter)
	{
		if (!lstrcmp((*iter)->GetName().data(), _name.data()))
		{
			return *iter;
		}
	}
	return nullptr;
}