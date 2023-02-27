#include "pch.h"
#include "CLevel.h"

CLevel::CLevel()
	:m_State{LEVEL_STATE::STOP}
{
	m_arrLayer[0].SetName(L"Default");

	for (int i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i].m_iLayerIdx = i;
	}
}

CLevel::~CLevel()
{

}

void CLevel::begin()
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		m_arrLayer[i].begin();
	}
}

void CLevel::tick()
{
	tick_function(m_arrLayer);
}

void CLevel::finaltick()
{
	finaltick_function(m_arrLayer);
}

void CLevel::render()
{
	render_function(m_arrLayer);
}

void CLevel::ClearLayer()
{
	clear_function(m_arrLayer);
}

CLayer* CLevel::GetLayer(const wstring& _strLayerName)
{
	for (auto iter{ m_arrLayer.begin() }; iter != m_arrLayer.end(); ++iter)
	{
		if (iter->GetName() == _strLayerName)
		{
			return &(*iter);
		}
	}
	return nullptr;
}

void CLevel::AddGameObject(CGameObject* _pObject, UINT _iLayerIdx)
{
	assert(0 <= _iLayerIdx && _iLayerIdx < MAX_LAYER);

	m_arrLayer[_iLayerIdx].AddGameObject(_pObject);
}

void CLevel::AddGameObject(CGameObject* _pObject, const wstring& _strLayerName)
{
	auto Layer = GetLayer(_strLayerName);
	Layer->AddGameObject(_pObject);
}

#include "CGameObject.h"

CGameObject* CLevel::FindParentObjectByName(const wstring& _Name)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i].GetParentObjects();

		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			if (_Name == vecObjects[j]->GetName())
				return vecObjects[j];
		}
	}

	return nullptr;
}

CGameObject* CLevel::FindObjectByName(const wstring& _Name)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObjects = m_arrLayer[i].GetParentObjects();

		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			if (_Name == vecObjects[j]->GetName())
				return vecObjects[j];

			const vector<CGameObject*>& vecChilds = vecObjects[j]->GetChilds();

			for (size_t k = 0; k < vecChilds.size(); ++k)
			{
				if (_Name == vecChilds[k]->GetName())
					return vecChilds[k];
			}
		}
	}

	return nullptr;
}

#include "CUIMgr.h"

CGameObject* CLevel::FindSelectedObject(const wstring& _Name)
{
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		const vector<CGameObject*>& vecObjects = CUIMgr::GetInst()->Get_Objects(UI_TYPE::GAMEOBJECT);

		for (size_t j = 0; j < vecObjects.size(); ++j)
		{
			if (vecObjects[j]->GetName() == L"SpotLight")
			{
				int a = 0;
			}
			if (_Name == vecObjects[j]->GetName())
				return vecObjects[j];
		}
	}
	return nullptr;
}