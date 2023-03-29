#include "pch.h"
#include "CEditor.h"
#include "imgui.h"

#include "CGameObjectEx.h"

#include "CImGuiMgr.h"

#include "ContentUI.h"

#include "CCameraScript.h"
#include "CBorderScript.h"
#include "CBrushScript.h"

#include <Engine\Base.h>

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

#include <Engine\CMRT.h>
#include <Engine\CSLight.h>

float g_LandScale = 1000.f;
float g_BrushScale = 0.3f;
int g_FaceCount = 16;

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

#include "TileMapUI.h"

void CEditor::init()
{
	CGameObjectEx* pObject{};

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
//	m_pCameraObject->SetType(OBJ_TYPE::EDIT);
	CRenderMgr::GetInst()->RegisterEditCam(m_pCameraObject->Camera());


	// LandScape 추가
	CGameObjectEx* pLandScape = new CGameObjectEx;
	pLandScape->SetName(L"EditLandScape");
	pLandScape->AddComponent(new CTransform);
	pLandScape->AddComponent(new CLandScape);
	g_BrushScale = pLandScape->LandScape()->GetBrushScale().x;
	pLandScape->Transform()->SetRelativeScale(g_LandScale, g_LandScale, g_LandScale);
	pLandScape->LandScape()->SetFaceCount(g_FaceCount, g_FaceCount);
	pLandScape->LandScape()->SetFrustumCulling(false);
	pLandScape->LandScape()->SetCameraObj((CGameObject*)m_pCameraObject);
	pLandScape->finaltick();
	//pLandScape->SetType(OBJ_TYPE::EDIT);
	m_EditorObj[(UINT)EDIT_MODE::MAPTOOL].emplace(L"EditLandScape", pLandScape);

	pObject = new CGameObjectEx;
	pObject->SetName(L"BrushObject");

	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CDecal);
	pObject->AddComponent(new CBrushScript);
	pObject->Transform()->SetRelativePos(Vec3(0.f, -200.f, 400.f));
	float fScale = g_BrushScale * g_LandScale * g_FaceCount;
	pObject->Transform()->SetRelativeScale(Vec3{ fScale, fScale, fScale });
	pObject->Transform()->SetRelativeRotation(XM_PI / 2.f, 0.f, 0.f);
	//pObject->Decal()->SetDecalTexture(CResMgr::GetInst()->FindRes<CTexture>(L"texture\\MagicCircle.png"));
	pObject->Decal()->SetDecalTexture(pLandScape->LandScape()->GetBrushTexture());
	pObject->Decal()->SetDefaultLit(true);
	m_EditorObj[(UINT)EDIT_MODE::MAPTOOL].emplace(L"BrushObject", pObject);

	TileMapUI* pLandScapeUI = (TileMapUI*)CImGuiMgr::GetInst()->FindUI("TileMapUI");
	pLandScapeUI->begin();
	ContentUI* pContentUI = (ContentUI*)CImGuiMgr::GetInst()->FindUI("ContentUI");
	pContentUI->begin();

	// Directional Light 추가
	CGameObjectEx* pDirLight = new CGameObjectEx;
	pDirLight->SetName(L"DirectionalLight");
	pDirLight->AddComponent(new CTransform);
	pDirLight->AddComponent(new CLight3D);

	pDirLight->Transform()->SetRelativePos(Vec3(-1000.f, 1000.f, 0.f));
	pDirLight->Transform()->SetRelativeRotation(Vec3(XM_PI / 2.f, 0.f, 0.f));

	pDirLight->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pDirLight->Light3D()->SetLightSpecular(Vec3(0.4f, 0.4f, 0.4f));
	pDirLight->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));
	pDirLight->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	pDirLight->Light3D()->SetLightDirection(Vec3(1.f, -1.f, 1.f));
	//pDirLight->SetType(OBJ_TYPE::EDIT);
	m_EditorObj[(UINT)EDIT_MODE::MAPTOOL].emplace(L"DirectionalLight", pDirLight);

	pObject = new CGameObjectEx;
	pObject->SetName(L"Sphere");
	pObject->AddComponent(new CTransform);
	pObject->AddComponent(new CMeshRender);
	pObject->AddComponent(new CCollider3D);

	pObject->Transform()->SetRelativePos(Vec3(0.f, 1000.f, 1000.f));
	pObject->Transform()->SetRelativeScale(Vec3(256.f, 256.f, 256.f));

	pObject->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	pObject->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DDeferredMtrl"), 0);
	pObject->MeshRender()->GetCurMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01.tga"));
	pObject->MeshRender()->GetCurMaterial(0)->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\tile\\TILE_01_N.tga"));
	m_EditorObj[(UINT)EDIT_MODE::MAPTOOL].emplace(L"Sphere", pObject);
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

void CEditor::tickObj()
{
	for (auto iter{ m_EditorObj[(UINT)m_editmode].begin() }; iter != m_EditorObj[(UINT)m_editmode].end(); ++iter)
	{
		if (L"EditLandScape" != iter->first)
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
	m_pCameraObject->tick();
}

void CEditor::finaltickObj()
{
	vector<CGameObject*> vec;

	for (auto iter{ m_EditorObj[(UINT)m_editmode].begin() }; iter != m_EditorObj[(UINT)m_editmode].end(); ++iter)
	{
		iter->second->finaltick();

		const vector<CScript*>& vecScript = iter->second->GetScripts();

		for (auto iter{ vecScript.begin() }; iter != vecScript.end(); ++iter)
		{
			(*iter)->finaltick();
		}
		vec.push_back(iter->second);
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

	m_pCameraObject->Camera()->SortObject(vec);
}

void CEditor::tick()
{
	tickObj();

	finaltickObj();

	picking();
}

void CEditor::picking()
{
	static Ray raycast{};

	if (KEY_PRESSED(KEY::LBTN))
	{
		Vec3 vPos{};
		const tRay& ray = m_pCameraObject->Camera()->GetRay();

		float fMaximum = 987654321.0f;
		raycast.direction = ray.vDir;
		raycast.position = ray.vStart;

		for (auto iter{ m_EditorObj[(UINT)m_editmode].begin() }; iter != m_EditorObj[(UINT)m_editmode].end(); ++iter)
		{
			if (nullptr != iter->second->Transform() && nullptr == iter->second->LandScape() && nullptr != iter->second->MeshRender())
			{
				if (iter->second->Transform()->Picking(raycast, vPos))
				{
					float fDist = (ray.vStart - vPos).Length();

					if (fMaximum > fDist)
					{
						fMaximum = fDist;
						CImGuiMgr::GetInst()->SetPickingObj(iter->second);
					}
				}
			}
		}
	}
}

#include <Engine\CRenderMgr.h>

void CEditor::render()
{
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
}

void CEditor::CreateDebugDrawObject()
{
	CGameObjectEx* pDebugObj = nullptr;

	// DEBUG_SHAPE::RECT
	pDebugObj = new CGameObjectEx;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh_Debug"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"), 0);
	pDebugObj->MeshRender()->SetInstancingType(INSTANCING_TYPE::USED);
	m_DebugDrawObject[(UINT)DEBUG_SHAPE::RECT] = pDebugObj;

	// DEBUG_SHAPE::CIRCLE
	pDebugObj = new CGameObjectEx;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CircleMesh_Debug"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"), 0);

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::CIRCLE] = pDebugObj;

	pDebugObj = new CGameObjectEx;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"CubeMesh"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"), 0);

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::CUBE] = pDebugObj;

	pDebugObj = new CGameObjectEx;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"), 0);

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::SPHERE] = pDebugObj;

	pDebugObj = new CGameObjectEx;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"ConeMesh"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"), 0);

	m_DebugDrawObject[(UINT)DEBUG_SHAPE::CONE] = pDebugObj;

	pDebugObj = new CGameObjectEx;

	pDebugObj->AddComponent(new CTransform);
	pDebugObj->AddComponent(new CMeshRender);

	pDebugObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"FrustumMesh"));
	pDebugObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"DebugDrawMtrl"), 0);

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

	pDebugObj->MeshRender()->GetCurMaterial(0)->SetScalarParam(SCALAR_PARAM::VEC4_0, &_info.vColor);

	pDebugObj->Transform()->finaltick();

	g_transform.matWorld = pDebugObj->Transform()->GetWorldMat();
	g_transform.matView = CRenderMgr::GetInst()->GetMainCam()->GetViewMat();
	g_transform.matProj = CRenderMgr::GetInst()->GetMainCam()->GetProjMat();

	pDebugObj->MeshRender()->SetInstancingType(INSTANCING_TYPE::NONE);
	//pDebugObj->render();
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
