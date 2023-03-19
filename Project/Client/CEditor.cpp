#include "pch.h"
#include "CEditor.h"
#include "imgui.h"

#include "CGameObjectEx.h"

#include "CCameraScript.h"
#include "CBorderScript.h"

#include <Engine/CTimeMgr.h>
#include <Engine\CMeshRender.h>

#include <Engine\CComponent.h>
#include <Engine\GlobalComponent.h>

#include <Engine\CCamera.h>

#include "CEditorCam.h"
#include "CCameraScript.h"

#include <Engine\CRenderMgr.h>
#include <Engine/CLevel.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CUitileMap.h>
#include <Engine\CAnimator2D.h>

#include <Engine\Base.h>

CEditor::CEditor()
	:m_editmode{ EDIT_MODE::MAPTOOL }
{
}

CEditor::~CEditor()
{

	for (auto iter{ m_EditorObj.begin() }; iter != m_EditorObj.end(); ++iter)
	{
		for (auto iter2{ iter->begin() }; iter2 != iter->end(); ++iter2)
		{
			Safe_Delete(iter2->second);
		}
	}
	Safe_Del_Array(m_DebugDrawObject);

	for (size_t i{}; i < (UINT)COMPONENT_TYPE::END; ++i)
		Safe_Delete(m_arrCom[i]);

	Safe_Delete(m_pCameraObject);
}

#include "CImGuiMgr.h"
#include "TileMapUI.h"

void CEditor::init()
{

	m_EditorObj.resize((UINT)EDIT_MODE::END);

	CreateDebugDrawObject();

	m_pCameraObject = new CGameObjectEx;
	m_pCameraObject->SetName(L"Editor Camera");

	m_pCameraObject->AddComponent(new CTransform);
	m_pCameraObject->AddComponent(new CCamera);
	m_pCameraObject->AddComponent(new CCameraScript);

	m_pCameraObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, -1000.f));

	m_pCameraObject->Camera()->SetProjType(PERSPECTIVE);
	m_pCameraObject->Camera()->SetLayerMask(1);
	CRenderMgr::GetInst()->RegisterEditCam(m_pCameraObject->Camera());

	// LandScape 추가
	CGameObjectEx* pLandScape = new CGameObjectEx;
	pLandScape->SetName(L"EditLandScape");

	pLandScape->AddComponent(new CTransform);
	pLandScape->AddComponent(new CLandScape);

	pLandScape->Transform()->SetRelativeScale(100.f, 100.f, 100.f);
	pLandScape->LandScape()->SetFaceCount(16, 16);
	pLandScape->LandScape()->SetFrustumCulling(false);
	m_EditorObj[(UINT)EDIT_MODE::MAPTOOL].emplace(L"EditLandScape", pLandScape);
	TileMapUI* pContentUI = (TileMapUI*)CImGuiMgr::GetInst()->FindUI("TileMapUI");
	pContentUI->begin();
	/*
	* Component List
	*/
	m_arrCom[(UINT)COMPONENT_TYPE::TRANSFORM] = new CTransform();
	m_arrCom[(UINT)COMPONENT_TYPE::TRANSFORM]->SetName(L"Transform");
	m_arrCom[(UINT)COMPONENT_TYPE::COLLIDER2D] = new CCollider2D();
	m_arrCom[(UINT)COMPONENT_TYPE::COLLIDER2D]->SetName(L"Colider2D");
	m_arrCom[(UINT)COMPONENT_TYPE::ANIMATOR2D] = new CAnimator2D();
	m_arrCom[(UINT)COMPONENT_TYPE::ANIMATOR2D]->SetName(L"Animator2D");
	//m_arrCom[(UINT)COMPONENT_TYPE::LIGHT2D]		= new CLight2D();
	//m_arrCom[(UINT)COMPONENT_TYPE::LIGHT2D]->SetName(L"Light2D");
	m_arrCom[(UINT)COMPONENT_TYPE::PARTICLESYSTEM] = new CParticleSystem();
	m_arrCom[(UINT)COMPONENT_TYPE::PARTICLESYSTEM]->SetName(L"CParticleSystem");
	m_arrCom[(UINT)COMPONENT_TYPE::LIGHT2D] = new CLight2D();
	m_arrCom[(UINT)COMPONENT_TYPE::LIGHT2D]->SetName(L"CLight2D");
	m_arrCom[(UINT)COMPONENT_TYPE::MESHRENDER] = new CMeshRender();
	m_arrCom[(UINT)COMPONENT_TYPE::MESHRENDER]->SetName(L"CMeshRender");
}

/*
* Editor 상태 분기
* progress, tick render
*/
void CEditor::progress()
{
	if (LEVEL_STATE::PLAY != CLevelMgr::GetInst()->GetCurLevel()->GetState())
	{
		tick();
		render();
	}
	//Debug Shape
	debug_render();
}

void CEditor::tick()
{
	m_pCameraObject->tick();

	for (auto iter{ m_EditorObj[(UINT)m_editmode].begin() }; iter != m_EditorObj[(UINT)m_editmode].end(); ++iter)
	{
		iter->second->tick();
	}

	switch (m_editmode)
	{
	case EDIT_MODE::ANIMATOR:
		break;
	case EDIT_MODE::MAPTOOL:
		break;
	case EDIT_MODE::OBJECT:
		break;
	case EDIT_MODE::END:
		break;
	default:
		break;
	}

	m_pCameraObject->finaltick();

	for (auto iter{ m_EditorObj[(UINT)m_editmode].begin() }; iter != m_EditorObj[(UINT)m_editmode].end(); ++iter)
	{
		iter->second->finaltick();
	}

	switch (m_editmode)
	{
	case EDIT_MODE::ANIMATOR:
		break;
	case EDIT_MODE::MAPTOOL:

		break;
	case EDIT_MODE::OBJECT:
		break;
	case EDIT_MODE::END:
		break;
	default:
		break;
	}
}

void CEditor::render()
{
	for (auto iter{ m_EditorObj[(UINT)m_editmode].begin() }; iter != m_EditorObj[(UINT)m_editmode].end(); ++iter)
	{
		iter->second->render();
	}

	switch (m_editmode)
	{
	case EDIT_MODE::ANIMATOR:
		if (m_pAnimationObject)
			m_pAnimationObject->render();
		break;
	case EDIT_MODE::MAPTOOL:
		//m_GirdObject->render();
		break;
	case EDIT_MODE::OBJECT:
		break;
	case EDIT_MODE::END:
		break;
	default:
		break;
	}

	m_pCameraObject->Camera()->render();
}

void CEditor::debug_render()
{
	// DebugDrawRender
// 일정 시간동안 렌더링 되는 Shape 
	list<tDebugShapeInfo>::iterator iter = m_DebugDrawList.begin();
	for (; iter != m_DebugDrawList.end(); )
	{
		iter->fCurTime += DT;
		if (iter->fDuration < iter->fCurTime)
		{
			iter = m_DebugDrawList.erase(iter);
		}
		else
		{
			DebugDraw(*iter);
			++iter;
		}
	}

	// 새로 추가된 DebugShape 확인
	vector<tDebugShapeInfo>& vecInfo = CRenderMgr::GetInst()->GetDebugDrawInfo();

	for (size_t i = 0; i < vecInfo.size(); ++i)
	{
		DebugDraw(vecInfo[i]);

		if (0.f < vecInfo[i].fDuration)
		{
			m_DebugDrawList.push_back(vecInfo[i]);
		}
	}
	vecInfo.clear();

	//CCamera* pCamera = CRenderMgr::GetInst()->GetMainCam();

	//if (pCamera)
	//{
	//	g_transform.matView = pCamera->GetViewMat();
	//	g_transform.matProj = pCamera->GetProjMat();
	//}
}

void CEditor::CreateDebugDrawObject()
{
	CGameObjectEx* pDebugObj = nullptr;

	// DEBUG_SHAPE::RECT
	pDebugObj = new CGameObjectEx;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh_Debug"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"));
	pDebugObj->MeshRender()->SetInstancingType(INSTANCING_TYPE::USED);
	m_DebugDrawObject[(UINT)DEBUG_SHAPE::RECT] = pDebugObj;

	// DEBUG_SHAPE::CIRCLE
	pDebugObj = new CGameObjectEx;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh_Debug"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"));

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::CIRCLE] = pDebugObj;

	pDebugObj = new CGameObjectEx;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"));

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::CUBE] = pDebugObj;

	pDebugObj = new CGameObjectEx;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"));

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::SPHERE] = pDebugObj;

	pDebugObj = new CGameObjectEx;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"ConeMesh"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"));

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::CONE] = pDebugObj;

	pDebugObj = new CGameObjectEx;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"FrustumMesh"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"));

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::FRUSTUM] = pDebugObj;
}


void CEditor::DebugDraw(tDebugShapeInfo& _info)
{
	CGameObjectEx* pDebugObj = m_DebugDrawObject[(UINT)_info.eShape];

	pDebugObj->Transform()->SetRelativePos(_info.vPosition);
	pDebugObj->Transform()->SetRelativeRotation(_info.vRot);

	if (DEBUG_SHAPE::CIRCLE == _info.eShape || DEBUG_SHAPE::SPHERE == _info.eShape)
	{
		pDebugObj->Transform()->SetRelativeScale(Vec3(_info.fRadius * 2.f, _info.fRadius * 2.f, _info.fRadius * 2.f));
	}
	else
	{
		pDebugObj->Transform()->SetRelativeScale(_info.vScale);
	}

	pDebugObj->MeshRender()->GetCurMaterial()->SetScalarParam(SCALAR_PARAM::VEC4_0, &_info.vColor);

	CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCam();

	pDebugObj->Transform()->finaltick();

	g_transform.matWorld = pDebugObj->Transform()->GetWorldMat();
	g_transform.matView = CRenderMgr::GetInst()->GetMainCam()->GetViewMat();
	g_transform.matProj = CRenderMgr::GetInst()->GetMainCam()->GetProjMat();

	pDebugObj->MeshRender()->SetInstancingType(INSTANCING_TYPE::NONE);
	pDebugObj->render();
}

void CEditor::Add_Editobject(EDIT_MODE _emode, CGameObjectEx* _pGameObject)
{
	const wstring& wstrName = _pGameObject->GetName();

	map<const wchar_t*, CGameObjectEx*>& map = m_EditorObj[(UINT)_emode];

	for (auto iter{ map.begin() }; iter != map.end(); ++iter)
	{
		if (!lstrcmp(wstrName.c_str(), iter->first))
			return;
	}

	map.emplace(make_pair(wstrName.c_str(), _pGameObject));
}

void CEditor::UpdateAnimationObject(CGameObject* _pGameObject)
{
	m_pAnimationObject = _pGameObject;
}

void CEditor::Add_Editobject(EDIT_MODE _emode, const wchar_t* _pName, CGameObjectEx* _pGameObject)
{
	map<const wchar_t*, CGameObjectEx*>& map = m_EditorObj[(UINT)_emode];

	for (auto iter{ map.begin() }; iter != map.end(); ++iter)
	{
		if (!lstrcmp(_pName, iter->first))
			return;
	}

	map.emplace(make_pair(_pName, _pGameObject));
}

CGameObjectEx* CEditor::FindByName(const wstring& _strky)
{

	if (!lstrcmp(_strky.c_str(), L"Editor Camera"))
	{
		return m_pCameraObject;
	}

	for (auto iter{ m_EditorObj.begin() }; iter != m_EditorObj.end(); ++iter)
	{
		for (auto iter2{ iter->begin() }; iter2 != iter->end(); ++iter2)
		{
			if (!lstrcmp(_strky.c_str(), iter2->first))
			{
				return iter2->second;
			}
		}
	}
	return nullptr;
}

void CEditor::PopByName(const wstring& _strky)
{

	for (auto iter{ m_EditorObj.begin() }; iter != m_EditorObj.end(); ++iter)
	{
		for (auto iter2{ iter->begin() }; iter2 != iter->end(); ++iter2)
		{
			if (!lstrcmp(_strky.c_str(), iter2->first))
			{
				iter->erase(iter2);
				break;
			}
		}
	}
}

void CEditor::SetEditMode(EDIT_MODE _editmode)
{
	assert(_editmode != EDIT_MODE::END);

	m_editmode = _editmode;

	Vec3 vPos{};
	CGameObjectEx* pGmaeObject{};

	switch (_editmode)
	{
	case EDIT_MODE::ANIMATOR:
	{
	}
	break;
	case EDIT_MODE::MAPTOOL:
		break;
	}
}

