#include "pch.h"

#include "CLevel.h"
#include "CLevelMgr.h"

#include "CDevice.h"
#include "CTexture.h"
#include "CResMgr.h"
#include "CRenderMgr.h"
#include "CCamera.h"

#include "CConstBuffer.h"
#include "CStructuredBuffer.h"

CRenderMgr::CRenderMgr()
	: m_pLight2DBuffer{}
	, m_pLight3DBuffer{}
{
	m_pLight2DBuffer = new CStructuredBuffer;
	m_pLight2DBuffer->Create(sizeof(tLightInfo), 2, SB_TYPE::SRV_ONLY, nullptr, true);

	m_pLight3DBuffer = new CStructuredBuffer();
	m_pLight3DBuffer->Create(sizeof(tLightInfo), 2, SB_TYPE::SRV_ONLY, nullptr, true);
}

CRenderMgr::~CRenderMgr()
{
	Safe_Delete(m_pLight2DBuffer);
	Safe_Delete(m_pLight3DBuffer);

	for (int i = 0; i < (UINT)MRT_TYPE::END; ++i)
	{
		if (nullptr != m_arrMRT[i])
		{
			Safe_Delete(m_arrMRT[i]);
		}
	}
}

void CRenderMgr::tick()
{
	m_vecCam.clear();
	//광원 정보 매 프레임 마다 삭제
	m_vecLight2D.clear();
	m_vecLight3D.clear();
}

void CRenderMgr::render()
{
	// MRT 타겟 클리어
	ClearMRT();

	UpdateNoiseTexture();

	UpdateLight2D();

	UpdateLight3D();

	static CConstBuffer* pGlobalCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	pGlobalCB->SetData(&g_global);
	pGlobalCB->UpdateData(PIPELINE_STAGE::ALL_STAGE);
	pGlobalCB->UpdateData_CS();
	
	//render_game();
	//GUI 주석
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();

	if (LEVEL_STATE::PLAY == pLevel->GetState())
	{
		render_game();
	}
	else
	{
		render_editor();
	}
	//GUI 주석 끝

}

void CRenderMgr::render_game()
{
	for (auto elem{ m_vecCam.begin() }; elem != m_vecCam.end(); ++elem)
	{
		(*elem)->render();
	}
}

void CRenderMgr::render_editor()
{
	assert(m_EditorCam);
	m_EditorCam->render();
}

void CRenderMgr::UpdateNoiseTexture()
{
	Ptr<CTexture> NoiseTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\noise\\noise_02.png");

	NoiseTex->UpdateData(58, PIPELINE_STAGE::ALL_STAGE);
	NoiseTex->UpdateData_CS(58, true);

	g_global.vNoiseResolution = Vec2{ NoiseTex->GetWidth(), NoiseTex->GetHeight() };
}

/*
* Light2D Component final Tick에서 Update된 m_vecLight2D 데이터를 구조체 버퍼 저장
*/
void CRenderMgr::UpdateLight2D()
{
	if (m_pLight2DBuffer->GetElementsCount() < m_vecLight2D.size())
	{
		m_pLight2DBuffer->Create(m_pLight2DBuffer->GetElementsSize(), (UINT)m_vecLight2D.size(), SB_TYPE::SRV_ONLY, nullptr, true);
	}
		
	/*
	* 구조체 버퍼안에, 조명 정보를 입력한다.
	*/
	static vector<tLightInfo> vecLight2D{};
	vecLight2D.clear();

	for (size_t i = 0; i < m_vecLight2D.size(); ++i)
	{
		vecLight2D.push_back(m_vecLight2D[i]->GetLightInfo());
	}

	m_pLight2DBuffer->SetData(vecLight2D.data(), (UINT)vecLight2D.size());

	m_pLight2DBuffer->UpdateData(55, PIPELINE_STAGE::ALL_STAGE);

	g_global.iLight2DCount = (UINT)vecLight2D.size();
}

void CRenderMgr::UpdateLight3D()
{
	if (m_pLight3DBuffer->GetElementsCount() < m_vecLight3D.size())
	{
		m_pLight3DBuffer->Create(m_pLight3DBuffer->GetElementsSize(), (UINT)m_vecLight3D.size(), SB_TYPE::SRV_ONLY, nullptr, true);
	}

	static vector<tLightInfo> vecLight3D{};
	vecLight3D.clear();

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		vecLight3D.push_back(m_vecLight3D[i]->GetLightInfo());
	}

	m_pLight3DBuffer->SetData(vecLight3D.data(), (UINT)vecLight3D.size());

	m_pLight3DBuffer->UpdateData(56, PIPELINE_STAGE::ALL_STAGE);

	g_global.iLight3DCount = (UINT)vecLight3D.size();
}

void CRenderMgr::CopyRenderTarget()
{
	static Ptr<CTexture> RTTex = CResMgr::GetInst()->FindRes<CTexture>(L"RenderTargetTex");
	
	ID3D11ShaderResourceView* SRV = nullptr;

	CONTEXT->PSSetShaderResources(60, 1, &SRV);

	CONTEXT->CopyResource(m_RTCopyTex->GetTex2D().Get(), RTTex->GetTex2D().Get());

	m_RTCopyTex->UpdateData(60, PIPELINE_STAGE::PS);
}

CCamera* CRenderMgr::GetMainCam()
{
	if (LEVEL_STATE::PLAY == CLevelMgr::GetInst()->GetLevelState())
	{
		if (m_vecCam.empty())
			return nullptr;

		return m_vecCam[1];
	}
	else
	{
		return m_EditorCam;
	}
}
