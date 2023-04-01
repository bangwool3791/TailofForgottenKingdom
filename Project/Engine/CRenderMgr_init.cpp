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

	float a = 4.f;
	pMtrl->SetScalarParam(FLOAT_1, &a);
	a = 0.6f;
	pMtrl->SetScalarParam(FLOAT_2, &a);

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DirLightMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));
	pMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"DataTargetTex"));
	pMtrl->SetTexParam(TEX_3, CResMgr::GetInst()->FindRes<CTexture>(L"FbxDiffuseTargetTex"));
	pMtrl->SetTexParam(TEX_4, CResMgr::GetInst()->FindRes<CTexture>(L"FbxSpecularTargetTex"));
	pMtrl->SetTexParam(TEX_5, CResMgr::GetInst()->FindRes<CTexture>(L"FbxAmbientTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));
	pMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"DataTargetTex"));
	pMtrl->SetTexParam(TEX_3, CResMgr::GetInst()->FindRes<CTexture>(L"FbxDiffuseTargetTex"));
	pMtrl->SetTexParam(TEX_4, CResMgr::GetInst()->FindRes<CTexture>(L"FbxSpecularTargetTex"));
	pMtrl->SetTexParam(TEX_5, CResMgr::GetInst()->FindRes<CTexture>(L"FbxAmbientTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"SpotLightMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"NormalTargetTex"));
	pMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"DataTargetTex"));
	pMtrl->SetTexParam(TEX_3, CResMgr::GetInst()->FindRes<CTexture>(L"FbxDiffuseTargetTex"));
	pMtrl->SetTexParam(TEX_4, CResMgr::GetInst()->FindRes<CTexture>(L"FbxSpecularTargetTex"));
	pMtrl->SetTexParam(TEX_5, CResMgr::GetInst()->FindRes<CTexture>(L"FbxAmbientTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DecalMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DeferredDecalMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"BlurMtrlX");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"BlurBloomTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	a = 1600.f;
	pMtrl->SetScalarParam(FLOAT_0, &a);

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"BlurMtrlY");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"BlurBloomTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	a = 900.f;
	pMtrl->SetScalarParam(FLOAT_0, &a);

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"BloomMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"HdrTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));
	Vec4 vec{0.5f, 0.5f, 0.5f, 0.f};
	pMtrl->SetScalarParam(VEC4_0, &vec);
	float fCos = 0.99f;
	pMtrl->SetScalarParam(FLOAT_0, &fCos);

	pMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"BloomUpdateMtrl");
	pMtrl->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"HdrTargetTex"));
	pMtrl->SetTexParam(TEX_1, CResMgr::GetInst()->FindRes<CTexture>(L"BlurTargetTex"));
	pMtrl->SetTexParam(TEX_2, CResMgr::GetInst()->FindRes<CTexture>(L"BloomTargetTex"));
	pMtrl->SetTexParam(TEX_3, CResMgr::GetInst()->FindRes<CTexture>(L"PositionTargetTex"));

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


			CResMgr::GetInst()->CreateTexture(L"FbxDiffuseTargetTex"
			, vRenderResolution.x, vRenderResolution.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),


			CResMgr::GetInst()->CreateTexture(L"FbxSpecularTargetTex"
			, vRenderResolution.x, vRenderResolution.y
			, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),


			CResMgr::GetInst()->CreateTexture(L"FbxAmbientTargetTex"
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

		Ptr<CTexture> pDSTex = CResMgr::GetInst()->FindRes<CTexture>(L"DepthStencilTex");

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

		// =========
		// Blur MRT
		// =========
		{
			Ptr<CTexture> arrRTTex[8] =
			{
				CResMgr::GetInst()->CreateTexture(L"BlurTargetTex"
				, vRenderResolution.x, vRenderResolution.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

			};

			Vec4 arrClear[8] = {
				  Vec4(0.f, 0.f, 0.f, 0.f)
				, Vec4(0.f, 0.f, 0.f, 0.f)
			};

			Ptr<CTexture> pDSTex = CResMgr::GetInst()->FindRes<CTexture>(L"DepthStencilTex");

			m_arrMRT[(UINT)MRT_TYPE::BLUR] = new CMRT;
			m_arrMRT[(UINT)MRT_TYPE::BLUR]->Create(arrRTTex, arrClear, pDSTex);
		}

		// =========
		// Bloom MRT
		// =========
		{
			Vec4 arrClear[8] = { Vec4(0.2f, 0.2f, 0.2f, 1.f), };

			Ptr<CTexture> arrRTTex[8] =
			{
				CResMgr::GetInst()->CreateTexture(L"BlurBloomTargetTex"
				, vRenderResolution.x, vRenderResolution.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			};

			Ptr<CTexture> pDSTex = nullptr;
			m_arrMRT[(UINT)MRT_TYPE::BLOOM] = new CMRT;
			m_arrMRT[(UINT)MRT_TYPE::BLOOM]->Create(arrRTTex, arrClear, pDSTex);
		}

		// =========
		// HDR MRT
		// =========
		{
			Vec4 arrClear[8] = { Vec4(0.2f, 0.2f, 0.2f, 1.f), };
			
			Ptr<CTexture> arrRTTex[8] =
			{
				CResMgr::GetInst()->CreateTexture(L"HdrTargetTex"
				, vRenderResolution.x, vRenderResolution.y
				, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			
			};
			
			Ptr<CTexture> pDSTex = nullptr;
			m_arrMRT[(UINT)MRT_TYPE::HDR] = new CMRT;
			m_arrMRT[(UINT)MRT_TYPE::HDR]->Create(arrRTTex, arrClear, pDSTex);
		}

		{
			CreateMRT(L"FrontTargetTex", MRT_TYPE::FRONT);
			CreateMRT(L"BackTargetTex", MRT_TYPE::BACK);
			CreateMRT(L"LeftTargetTex", MRT_TYPE::LEFT);
			CreateMRT(L"RightTargetTex", MRT_TYPE::RIGHT);
			CreateMRT(L"UpTargetTex", MRT_TYPE::UP);
			CreateMRT(L"DownTargetTex", MRT_TYPE::DOWN);
		}

		// =============
		// ShadowMap MRT
		// =============
		{
			Ptr<CTexture> arrRTTex[8] =
			{
				CResMgr::GetInst()->CreateTexture(L"DepthMapTex"
												, 4096 * 2, 4096 * 2
												, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),
			};

			Vec4 arrClear[8] = {
					 Vec4(0.f, 0.f, 0.f, 0.f)
			};

			// Depth Stencil Texture 만들기
			Ptr<CTexture> pDepthStencilTex
				= CResMgr::GetInst()->CreateTexture(L"DepthMapDSTex"
					, 4096 * 2, 4096 * 2
					, DXGI_FORMAT_D32_FLOAT, D3D11_BIND_DEPTH_STENCIL);


			m_arrMRT[(UINT)MRT_TYPE::SHADOW] = new CMRT;
			m_arrMRT[(UINT)MRT_TYPE::SHADOW]->Create(arrRTTex, arrClear, pDepthStencilTex);
		}
	}
}

void CRenderMgr::CreateMRT(const wstring& wstr, MRT_TYPE eType)
{
	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();

	Vec4 arrClear[8] = { Vec4(0.2f, 0.2f, 0.2f, 1.f), };

	Ptr<CTexture> arrRTTex[8] =
	{
		CResMgr::GetInst()->CreateTexture(wstr
		, vRenderResolution.x, vRenderResolution.y
		, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE),

	};

	Ptr<CTexture> pDSTex = CResMgr::GetInst()->FindRes<CTexture>(L"CubeDepthStencilTex");

	if (nullptr == pDSTex)
	{
		pDSTex
			= CResMgr::GetInst()->CreateTexture(L"CubeDepthStencilTex"
				, vRenderResolution.x, vRenderResolution.y
				, DXGI_FORMAT_D32_FLOAT, D3D11_BIND_DEPTH_STENCIL);
	}
	m_arrMRT[(UINT)eType] = new CMRT;
	m_arrMRT[(UINT)eType]->Create(arrRTTex, arrClear, pDSTex);
}

void CRenderMgr::ClearMRT()
{
	for (int i = 0; i < (UINT)MRT_TYPE::END; ++i)
	{
		if ((UINT)MRT_TYPE::FRONT == i)
			continue;
		if ((UINT)MRT_TYPE::BACK == i)
			continue;
		if ((UINT)MRT_TYPE::LEFT == i)
			continue;
		if ((UINT)MRT_TYPE::RIGHT == i)
			continue;
		if ((UINT)MRT_TYPE::UP == i)
			continue;
		if ((UINT)MRT_TYPE::DOWN == i)
			continue;

		if (nullptr != m_arrMRT[i])
		{
			m_arrMRT[i]->Clear();
		}
	}
}

void CRenderMgr::ClearMRT(MRT_TYPE _eType)
{
	m_arrMRT[(UINT)_eType]->Clear();
}