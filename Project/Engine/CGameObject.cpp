#include "pch.h"

#include "CMesh.h"
#include "CGraphicsShader.h"

#include "CLevel.h"
#include "CLayer.h"
#include "CLevelMgr.h"

#include "CGameObject.h"
#include "CEventMgr.h"

#include "CComponent.h"
#include "CRenderComponent.h"
#include "CMeshRender.h"
#include "CScript.h"
#include "CCamera.h"


template<typename T>
void Tick(T* obj) { obj->tick(); }

template<typename T>
void FinalTick(T* obj) { obj->finaltick(); }

CGameObject::CGameObject()
	:m_arrCom{}
	, m_iLayerIdx{ -1 }
	, m_pParent{ nullptr }
	, m_pRenderComponent{ nullptr }
	, m_vecChild{}
	, m_vecScripts{}
{
}

CGameObject::CGameObject(const CGameObject& rhs)
	:CEntity(rhs)
	, m_pParent(nullptr)
	, m_arrCom{}
	, m_pRenderComponent(nullptr)
	, m_iLayerIdx(-1)
{
	for (size_t i{ 0 }; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (rhs.m_arrCom[i] == nullptr)
			continue;

		AddComponent(rhs.m_arrCom[i]->Clone());
	}

	for (size_t i{ 0 }; i < rhs.m_vecScripts.size(); ++i)
	{
		AddComponent(rhs.m_vecScripts[i]->Clone());
	}

	for (size_t i{ 0 }; i < rhs.m_vecChild.size(); ++i)
	{
		AddChild(rhs.m_vecChild[i]->Clone());
	}
	//복사 생성자에서 비긴 삭제
}

CGameObject::~CGameObject()
{
	for (int i{ 0 }; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (m_arrCom[i])
			delete m_arrCom[i];

		m_arrCom[i] = nullptr;
	}

	for (auto iter{ m_vecScripts.begin() }; iter != m_vecScripts.end(); ++iter)
	{
		delete* iter;
		*iter = nullptr;
	}

	Safe_Del_Vec(m_vecChild);
}

void CGameObject::begin()
{

	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (i == (UINT)COMPONENT_TYPE::CAMERA)
		{
			int a = 0;
		}
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->begin();
	}

	for (size_t i = 0; i < m_vecScripts.size(); ++i)
	{
		m_vecScripts[i]->begin();
	}

	for (auto Iter{ m_vecChild.begin() }; Iter != m_vecChild.end(); ++Iter)
	{
		if ((*Iter))
			(*Iter)->begin();
	}

}

void CGameObject::tick()
{

	for (auto Iter{ m_arrCom.begin() }; Iter != m_arrCom.end(); ++Iter)
	{
		if ((*Iter))
			(*Iter)->tick();
	}

	for (auto Iter{ m_vecScripts.begin() }; Iter != m_vecScripts.end(); ++Iter)
	{
		if ((*Iter))
			(*Iter)->tick();
	}

	for (auto Iter{ m_vecChild.begin() }; Iter != m_vecChild.end(); ++Iter)
	{
		if ((*Iter))
			(*Iter)->tick();
	}
}

void CGameObject::finaltick()
{

	for (auto Iter{ m_arrCom.begin() }; Iter != m_arrCom.end(); ++Iter)
	{
		if ((*Iter))
			(*Iter)->finaltick();
	}

	for (auto Iter{ m_vecScripts.begin() }; Iter != m_vecScripts.end(); ++Iter)
	{
		if ((*Iter))
			(*Iter)->finaltick();
	}

	for (auto Iter{ m_vecChild.begin() }; Iter != m_vecChild.end(); ++Iter)
	{
		if ((*Iter))
			(*Iter)->finaltick();
	}

	for (auto iter{ m_vecChild.begin() }; iter != m_vecChild.end();)
	{
		if ((*iter)->IsDead())
		{
			iter = m_vecChild.erase(iter);
		}
		else
		{
			++iter;
		}
	}
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();
	CLayer* pLayer = pLevel->GetLayer(m_iLayerIdx);
	pLayer->RegisterObject(this);
}

void CGameObject::finaltick_module()
{
	// Component
	for (UINT i = 0; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (nullptr != m_arrCom[i])
			m_arrCom[i]->finaltick_module();
	}

	// Child Object
	vector<CGameObject*>::iterator iter = m_vecChild.begin();
	for (; iter != m_vecChild.end();)
	{
		(*iter)->finaltick_module();

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

void CGameObject::render()
{
	if (nullptr == m_pRenderComponent)
		return;

	m_pRenderComponent->render();
}

void CGameObject::AddComponent(CComponent* _pComponent)
{

	auto eComType = _pComponent->GetType();

	if (eComType != COMPONENT_TYPE::SCRIPT)
	{
		if (m_arrCom[(UINT)eComType])
		{
			Safe_Delete(m_arrCom[(UINT)eComType]);
		}
	}

	_pComponent->m_pOwnerObject = this;

	//[220930] 아래 코드 없을 때, m_arrCom에서 포인터 못찾음
	switch (eComType)
	{
	case COMPONENT_TYPE::SCRIPT:
		m_vecScripts.push_back((CScript*)_pComponent);
		break;
	default:
		auto pRenderCom = dynamic_cast<CRenderComponent*>(_pComponent);

		if (nullptr != pRenderCom)
		{
			if (m_pRenderComponent)
			{
				MessageBox(nullptr, L"m_pRenderComponent != nullptr", L"에러", MB_OK);
				return;
			}
			m_pRenderComponent = pRenderCom;
		}

		if (nullptr != m_arrCom[(UINT)eComType])
		{
			delete m_arrCom[(UINT)eComType];
			m_arrCom[(UINT)eComType] = nullptr;
		}

		m_arrCom[(UINT)eComType] = _pComponent;
		break;
	}
}

void CGameObject::AddChild(CGameObject* _pChild)
{
	/*
	* 최상위 부모라면
	*/
	if (nullptr == _pChild->Get_Parent())
	{
		/*
		* 레이어 미 정의
		*/
		if (-1 == _pChild->m_iLayerIdx)
		{
			//자식의 현재 레이어 = 부모레이어
			_pChild->m_iLayerIdx = m_iLayerIdx;
		}
		else
		{
			//현재 레이어, 자식 객체 등록 해제
			CLayer* pLayer = CLevelMgr::GetInst()->GetCurLevel()->GetLayer(_pChild->m_iLayerIdx);
			pLayer->DeregisterObject(_pChild);
		}
	}
	//하위 자식
	else
	{
		//부모 연결 해지
		_pChild->DisconnectFromParent();
	}
	_pChild->m_pParent = this;
	m_vecChild.push_back(_pChild);
}

CGameObject* CGameObject::GetChild(const wstring& _name)
{
	for (auto iter{ m_vecChild.begin() }; iter != m_vecChild.end(); ++iter)
	{
		if (!lstrcmp(_name.c_str(), (*iter)->GetName().c_str()))
		{
			return (*iter);
		}
	}
	return nullptr;
}

void CGameObject::DestroyComponent(COMPONENT_TYPE _eComType)
{
	if (m_arrCom[(UINT)_eComType])
	{
		delete m_arrCom[(UINT)_eComType];
		m_arrCom[(UINT)_eComType] = nullptr;
	}

	if (COMPONENT_TYPE::MESHRENDER == _eComType)
		m_pRenderComponent = nullptr;
}

CComponent* CGameObject::GetComponent(COMPONENT_TYPE _eComType)
{
	return m_arrCom[(UINT)_eComType];
}

void CGameObject::Destroy()
{
	tEvent eve{};
	eve.eType = EVENT_TYPE::DELETE_OBJECT;
	eve.wParam = (DWORD_PTR)this;

	CEventMgr::GetInst()->AddEvent(eve);
}

void CGameObject::DisconnectFromParent()
{
	vector<CGameObject*>::iterator iter = m_pParent->m_vecChild.begin();

	for (; iter != m_pParent->m_vecChild.end(); ++iter)
	{
		if ((*iter) == this)
		{
			m_pParent->m_vecChild.erase(iter);
			m_pParent = nullptr;
			return;
		}
	}

	assert(nullptr);
}

bool CGameObject::DeleteComponent(const string& _strName)
{
	wstring wstrName = wstring(_strName.begin(), _strName.end());

	for (size_t i{}; i < m_arrCom.size(); ++i)
	{
		if(nullptr != (m_arrCom[i]))
			if (!lstrcmp(wstrName.c_str(), (m_arrCom[i])->GetName().c_str()))
			{
				//Engine Progess 완료 후 
				delete m_arrCom[i];
				m_arrCom[i] = nullptr;
				return true;
			}
	}
	return false;
}

bool CGameObject::DeleteScript(const string& _strName)
{
	wstring wstrName = wstring(_strName.begin(), _strName.end());

	for (auto iter{ m_vecScripts.begin() }; iter != m_vecScripts.end(); ++iter)
	{
		if (nullptr != (*iter))
			if (!lstrcmp(wstrName.c_str(), (*iter)->GetName().c_str()))
			{
				//Engine Progess 완료 후 
				delete (*iter);
				m_vecScripts.erase(iter);
				return true;
			}
	}
	return false;
}