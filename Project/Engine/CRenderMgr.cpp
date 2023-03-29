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

#include "CMRT.h"

void CRenderMgr::tick()
{
	m_vecCam.clear();
	//���� ���� �� ������ ���� ����
	m_vecLight2D.clear();
	m_vecLight3D.clear();
}

void CRenderMgr::render()
{
	clear();
	//render_game();
	//GUI �ּ�
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();

	if (LEVEL_STATE::PLAY == pLevel->GetState())
	{
		render_game();
	}
	else
	{
		render_editor();
	}
	//GUI �ּ� ��
}

void CRenderMgr::render(const vector<CGameObject*>& obj)
{
	clear();
	//render_game();
	//GUI �ּ�
	CLevel* pLevel = CLevelMgr::GetInst()->GetCurLevel();

	if (LEVEL_STATE::PLAY == pLevel->GetState())
	{
		render_game();
	}
	else
	{
		render_editor(obj);
	}
	//GUI �ּ� ��
}

void CRenderMgr::clear()
{
	// MRT Ÿ�� Ŭ����
	ClearMRT();

	UpdateNoiseTexture();

	UpdateLight2D();

	UpdateLight3D();

	static CConstBuffer* pGlobalCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::GLOBAL);
	pGlobalCB->SetData(&g_global);
	pGlobalCB->UpdateData(PIPELINE_STAGE::ALL_STAGE);
	pGlobalCB->UpdateData_CS();
}

#include "CLevelMgr.h"

void CRenderMgr::render_game()
{
	render_dynamic_shadowdepth();

	for (auto elem{ m_vecCam.begin() }; elem != m_vecCam.end(); ++elem)
	{
		const wstring& wstr = (*elem)->GetOwner()->GetName();

		(*elem)->render();
	}
}


void CRenderMgr::render_dynamic_shadowdepth()
{
	m_arrMRT[(UINT)MRT_TYPE::SHADOW]->OMSet();

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		if (LIGHT_TYPE::DIRECTIONAL == m_vecLight3D[i]->GetLightType())
			m_vecLight3D[i]->render_depthmap();
	}
}

void CRenderMgr::render_dynamic_shadowdepth(const vector<CGameObject*>& obj)
{
	m_arrMRT[(UINT)MRT_TYPE::SHADOW]->OMSet();

	for (size_t i = 0; i < m_vecLight3D.size(); ++i)
	{
		if (LIGHT_TYPE::DIRECTIONAL == m_vecLight3D[i]->GetLightType())
			m_vecLight3D[i]->render_depthmap(obj);
	}
}

void CRenderMgr::render_editor()
{
	assert(m_EditorCam);
	render_dynamic_shadowdepth();

	m_EditorCam->EditorRender();
}

void CRenderMgr::render_editor(const vector<CGameObject*>& obj)
{
	assert(m_EditorCam);
	render_dynamic_shadowdepth(obj);

	m_EditorCam->EditorRender();
}


void CRenderMgr::UpdateNoiseTexture()
{
	Ptr<CTexture> NoiseTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\noise\\noise_02.png");

	NoiseTex->UpdateData(58, PIPELINE_STAGE::ALL_STAGE);
	NoiseTex->UpdateData_CS(58, true);

	g_global.vNoiseResolution = Vec2{ NoiseTex->GetWidth(), NoiseTex->GetHeight() };
}

/*
* Light2D Component final Tick���� Update�� m_vecLight2D �����͸� ����ü ���� ����
*/
void CRenderMgr::UpdateLight2D()
{
	if (m_pLight2DBuffer->GetElementCount() < m_vecLight2D.size())
	{
		m_pLight2DBuffer->Create(m_pLight2DBuffer->GetElementsSize(), (UINT)m_vecLight2D.size(), SB_TYPE::SRV_ONLY, nullptr, true);
	}
		
	/*
	* ����ü ���۾ȿ�, ���� ������ �Է��Ѵ�.
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
	if (m_pLight3DBuffer->GetElementCount() < m_vecLight3D.size())
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

		return m_vecCam[0];
	}
	else
	{
		return m_EditorCam;
	}
}
