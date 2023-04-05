#include "pch.h"
#include "CLoadingLevel.h"

#include <Engine\CRenderMgr.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>

#include <Engine\CDevice.h>
#include <Engine\CGameObject.h>
#include <Engine\CTransform.h>
#include <Engine\CCamera.h>
#include <Engine\CMeshRender.h>
#include <Engine\CLight3D.h>
#include "CLoader.h"

CLoadingLevel::CLoadingLevel()
{

}

CLoadingLevel::~CLoadingLevel()
{

}

void CLoadingLevel::init()
{
	CSaveLoadMgr::GetInst()->init();
	CreateTestLelvel();
	CEditor::GetInst()->init();

	m_pLoader = new CLoader;
	m_pLoader->Initialize();

	CreateUI();
}

void CLoadingLevel::progress()
{
	CEngine::GetInst()->progress();

	CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();

	if (LEVEL_STATE::STOP == pLevel->GetState()
		|| LEVEL_STATE::PAUSE == pLevel->GetState())
	{
		CEditor::GetInst()->progress();

		auto map = CEditor::GetInst()->GetRenderObj();

		for (auto iter = map.begin(); iter != map.end(); ++iter)
		{
			m_vecEditoShadowObj.push_back((CGameObject*)iter->second);
		}
	}
	else if (LEVEL_STATE::LOADER == pLevel->GetState())
	{
		if (m_pLoader->Get_isFinished())
			pLevel->SetState(LEVEL_STATE::PAUSE);

		m_pLoderCamearObj->tick();
		m_pLoderUIObj->tick();
		m_pLoderCamearObj->finaltick_module();
		m_pLoderUIObj->finaltick_module();

		CRenderMgr::GetInst()->RegisterLight3D(m_pLightObj->Light3D());

	}
	else
	{

	}
}

void CLoadingLevel::render()
{
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();


	if (LEVEL_STATE::STOP == pLevel->GetState()
		|| LEVEL_STATE::PLAY == pLevel->GetState()
		|| LEVEL_STATE::PAUSE == pLevel->GetState())
	{
		CEditor::GetInst()->debug_render();
		m_vecEditoShadowObj.clear();

		CImGuiMgr::GetInst()->progress();
	}

	// Present(SwapChain)
	CDevice::GetInst()->Present();
}

void CLoadingLevel::CreateUI()
{
	m_pLoderCamearObj = new CGameObject;
	m_pLoderCamearObj->SetName(L"Loader");

	m_pLoderCamearObj->AddComponent(new CTransform);
	m_pLoderCamearObj->AddComponent(new CCamera);
	m_pLoderCamearObj->Transform()->SetRelativePos(Vec3(0.f, 0.f, -1000.f));
	m_pLoderCamearObj->Camera()->SetProjType(ORTHOGRAHPIC);

	m_pCameraLoader = m_pLoderCamearObj->Camera();

	m_pLoderUIObj = new CGameObject;
	m_pLoderUIObj->SetName(L"Rect");

	m_pLoderUIObj->AddComponent(new CTransform);
	m_pLoderUIObj->AddComponent(new CMeshRender);

	Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
	m_pLoderUIObj->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
	m_pLoderUIObj->Transform()->SetRelativeScale(Vec3(vResolution.x, vResolution.y, 0.f));

	m_pLoderUIObj->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
	m_pLoderUIObj->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"Std3DMtrl"), 0);
	m_pLoderUIObj->MeshRender()->GetCurMaterial(0)->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\UI\\IntroUI.jpg"));

	m_pLightObj = new CGameObject;
	m_pLightObj->SetName(L"DirectionalLight");
	m_pLightObj->AddComponent(new CTransform);
	m_pLightObj->AddComponent(new CLight3D);
	
	m_pLightObj->Transform()->SetRelativePos(Vec3(-1000.f, 1000.f, 0.f));
	m_pLightObj->Transform()->SetRelativeRotation(Vec3(XM_PI / 4.f, 0.f, 0.f));
	
	m_pLightObj->Light3D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	m_pLightObj->Light3D()->SetLightSpecular(Vec3(0.4f, 0.4f, 0.4f));
	m_pLightObj->Light3D()->SetLightAmbient(Vec3(0.15f, 0.15f, 0.15f));
	m_pLightObj->Light3D()->SetLightType(LIGHT_TYPE::DIRECTIONAL);
	m_pLightObj->Light3D()->SetLightDirection(Vec3(1.f, -1.f, 1.f));
}
