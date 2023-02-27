#include "pch.h"
#include "CEventMgr.h"

#include "CGameObject.h"
#include "CLevel.h"
#include "CLayer.h"
#include "CLevelMgr.h"

CEventMgr::CEventMgr()
{

}

CEventMgr::~CEventMgr()
{

}

void CEventMgr::tick()
{
	m_bLevelChanged = false;

	for (UINT i{}; i < m_vecGarbage.size(); ++i)
	{
		delete m_vecGarbage[i];
		m_bLevelChanged = true;
	}
	m_vecGarbage.clear();

	for (UINT i{}; i < m_vecEvent.size(); ++i)
	{
		switch (m_vecEvent[i].eType)
		{
		case EVENT_TYPE::CREATE_OBJECT:
		{
			CGameObject* pGameObeject = (CGameObject*)m_vecEvent[i].wParam;
			int	iLayer = (int)m_vecEvent[i].lParam;
			CLevel* level = CLevelMgr::GetInst()->GetCurLevel();
			pGameObeject->begin();
			level->AddGameObject(pGameObeject, iLayer);
			m_bLevelChanged = true;
		}
			break;
		case EVENT_TYPE::ADD_CHILD:
		{
			// wParam : Child Adress, lParam : Parent Adress
			CGameObject* pParent = (CGameObject*)m_vecEvent[i].lParam;
			CGameObject* pChild = (CGameObject*)m_vecEvent[i].wParam;
			pChild->begin();
			pParent->AddChild(pChild);
			m_bLevelChanged = true;
		}
			break;
		case EVENT_TYPE::DELETE_OBJECT:
		{
			static queue<CGameObject*> que;
			CGameObject* pGameObj = (CGameObject*)m_vecEvent[i].wParam;
			if (!pGameObj->IsDead())
			{
				que.push(pGameObj);

				while (!que.empty())
				{
					CGameObject* pObj = (CGameObject*)que.front();
					que.pop();
					m_vecGarbage.push_back(pObj);

					vector<CGameObject*> vecChild = pObj->GetChilds();

					for (UINT j{}; j <  vecChild.size(); ++j)
					{
						que.push(vecChild[j]);
					}

					pObj->m_bDead = true;
				}
			}
		}
			break;
		case EVENT_TYPE::CHANGE_LEVEL:
		{
			m_bLevelChanged = true;
		}
			break;
		case EVENT_TYPE::CHANGE_LEVEL_STATE:
		{
			m_bLevelChanged = true;
			CLevelMgr::GetInst()->ChangeLevelState((LEVEL_STATE)m_vecEvent[i].wParam);
		}
			break;

		case EVENT_TYPE::EDIT_RES:
		{
			switch ((RES_TYPE)m_vecEvent[i].wParam)
			{
			case RES_TYPE::PREFAB:
			{
				if (((CRes*)m_vecEvent[i].lParam)->GetName().empty())
					break;

				wchar_t sz_data[255] = {};
				lstrcpy(sz_data, ((CRes*)m_vecEvent[i].lParam)->GetName().c_str());
				wstring wstrRelativePath = lstrcat(sz_data, L"Prefab");
				CResMgr::GetInst()->AddRes(wstrRelativePath, new CPrefab(((CGameObject*)m_vecEvent[i].lParam)->Clone(), false));
			}
				break;
			case RES_TYPE::COMPUTE_SHADER:
				CResMgr::GetInst()->AddRes(((CRes*)m_vecEvent[i].lParam)->GetName(), (CComputeShader*)m_vecEvent[i].lParam);
				break;
			case RES_TYPE::MATERIAL:
				CResMgr::GetInst()->AddRes<CMaterial>(((CRes*)m_vecEvent[i].lParam)->GetName(), (CMaterial*)(m_vecEvent[i].lParam));
				break;
			case RES_TYPE::MESH:
				CResMgr::GetInst()->AddRes(((CRes*)m_vecEvent[i].lParam)->GetName(), (CMesh*)m_vecEvent[i].lParam);
				break;
			case RES_TYPE::TEXTURE:
				CResMgr::GetInst()->AddRes(((CRes*)m_vecEvent[i].lParam)->GetName(), (CTexture*)m_vecEvent[i].lParam);
				break;
			default:
				break;
			}
		}
			break;
		case EVENT_TYPE::DELETE_RES:
		{
			// wParam : RES_TYPE, lParam : Resource Adress
			if (!CResMgr::GetInst()->DeleteRes((RES_TYPE)m_vecEvent[i].wParam, ((CRes*)m_vecEvent[i].lParam)->GetKey()))
			{
				MessageBox(nullptr, L"리소스 삭제 실패", L"에러", MB_OK);
			}
			break;
		}
		}
	}

	m_vecEvent.clear();
}

