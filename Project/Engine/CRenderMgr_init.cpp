#include "pch.h"
#include "CRenderMgr.h"

#include "CDevice.h"
#include "CMRT.h"

void CRenderMgr::init()
{
	Vec2 vRenderResol = CDevice::GetInst()->GetRenderResolution();

	// 후처리 용도, 렌더타겟 복사용
	m_RTCopyTex = CResMgr::GetInst()->CreateTexture(L"RTCopyTex"
		, (UINT)vRenderResol.x, (UINT)vRenderResol.y
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_SHADER_RESOURCE);

	m_RTCopyTex->UpdateData(60, PIPELINE_STAGE::PS);

	CreateMRT();

	// MergeMtrl 에 파라미터 세팅
	Ptr<CMaterial> pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"MergeMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"ColorTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"DiffuseTargetTex"));
	pMtrl->SetTexParam(TEX_3, CResMgr::GetInst()->FindRes<CTexture>(L"SpecularTargetTex"));
	pMtrl->SetTexParam(TEX_4, CResMgr::GetInst()->FindRes<CTexture>(L"DecalTargetTex"));

	float a = 1.f;
	pMtrl->SetScalarParam(FLOAT_1, &a);
	pMtrl->SetScalarParam(FLOAT_2, &a);

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DirLightMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));
	pMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"DataTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));
	pMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"DataTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"SpotLightMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));
	pMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"DataTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DecalMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DeferredDecalMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
}

void CRenderMgr::CreateMRT()
{
	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();

	// =============
	// Swapchain MRT
	// =============

	{
		Ptr<CTexture> arrRTTex[8] = {};
		Vec4 arrClear[8] = { Vec4(0.2f, 0.2f, 0.2f, 1.f), };

		Ptr<CTexture> pDSTex = nullptr;

		arrRTTex[0] = CResMgr::GetInst()->FindRes<CTexture>(L"RenderTargetTex");
		pDSTex = CResMgr::GetInst()->FindRes<CTexture>(L"DepthStencilTex");

		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::SWAPCHAIN]->Create(arrRTTex, arrClear, pDSTex);
	}

	// ============
	// Deferred MRT
	// ============
	{
		Ptr<CTexture> arrRTTex[8] =
		{
			CResMgr::GetInst()->CreateTexture(L"ColorTargetTex"
			, vRenderResolution.x, vRenderResolution.y
			, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		
			//Color 제외 Texture Vec4
			CResMgr::GetInst()->CreateTexture(L"NormalTargetTex"
			, vRenderResolution.x, vRenderResolution.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			CResMgr::GetInst()->CreateTexture(L"PositionTargetTex"
			, vRenderResolution.x, vRenderResolution.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			CResMgr::GetInst()->CreateTexture(L"DataTargetTex"
			, vRenderResolution.x, vRenderResolution.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			/*CResMgr::GetInst()->CreateTexture(L"EmissiveTargetTex"
								, vRenderResolution.x, vRenderResolution.y
								, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),*/
		};

		Vec4 arrClear[8] = {
		  Vec4(0.f, 0.f, 0.f, 0.f)
		, Vec4(0.f, 0.f, 0.f, 0.f)
		, Vec4(0.f, 0.f, 0.f, 0.f)
		, Vec4(0.f, 0.f, 0.f, 0.f)
		};

		Ptr<CTexture> pDSTex = CResMgr::GetInst()->FindRes<CTexture>(L"DepthStencilTex");

		m_arrMRT[(UINT)MRT_TYPE::DEFERRED] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::DEFERRED]->Create(arrRTTex, arrClear, pDSTex);
	}

	// =========
	// Light MRT
	// =========
	{
		Ptr<CTexture> arrRTTex[8] =
		{
			CResMgr::GetInst()->CreateTexture(L"DiffuseTargetTex"
											, vRenderResolution.x, vRenderResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS),
			CResMgr::GetInst()->CreateTexture(L"SpecularTargetTex"
											, vRenderResolution.x, vRenderResolution.y
											, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
		};

		Vec4 arrClear[8] = {
			  Vec4(0.f, 0.f, 0.f, 0.f)
			, Vec4(0.f, 0.f, 0.f, 0.f)
		};

		Ptr<CTexture> pDSTex = nullptr;

		m_arrMRT[(UINT)MRT_TYPE::LIGHT] = new CMRT;
		m_arrMRT[(UINT)MRT_TYPE::LIGHT]->Create(arrRTTex, arrClear, pDSTex);

		// =========
		// Decal MRT
		// =========
		{
			Ptr<CTexture> arrRTTex[8] =
			{
				CResMgr::GetInst()->CreateTexture(L"DecalTargetTex"
				, vRenderResolution.x, vRenderResolution.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			};

			Vec4 arrClear[8] = {
				  Vec4(0.f, 0.f, 0.f, 0.f)
				, Vec4(0.f, 0.f, 0.f, 0.f)
			};

			Ptr<CTexture> pDSTex = nullptr;

			m_arrMRT[(UINT)MRT_TYPE::DECAL] = new CMRT;
			m_arrMRT[(UINT)MRT_TYPE::DECAL]->Create(arrRTTex, arrClear, pDSTex);
		}
	}
}

void CRenderMgr::ClearMRT()
{
	for (int i = 0; i < (UINT)MRT_TYPE::END; ++i)
	{
		if (nullptr != m_arrMRT[i])
		{
			m_arrMRT[i]->Clear();
		}
	}
}