#include "pch.h"
#include "CLandScape.h"

#include "CResMgr.h"
#include "CEventMgr.h"


void CLandScape::CreateMesh()
{
	// 지형 메쉬 설정
	CMesh* pMesh = GetMesh().Get();

	// 메쉬 만들기
	// 기존에 참조하던 메쉬는 삭제
	if (nullptr != pMesh)
	{
		DeleteRes(pMesh, RES_TYPE::MESH);
	}

	vector<Vtx> vecVtx;
	vector<UINT> vecIdx;

	Vtx v;

	 //정점 배치
	for (UINT row = 0; row < m_iZFaceCount + 1; ++row)
	{
		for (UINT col = 0; col < m_iXFaceCount + 1; ++col)
		{
			v.vPos = Vec3((float)col, 0.f, (float)row);
			v.vUV = Vec2(col, m_iZFaceCount - row);
	
			v.vNormal = Vec3(0.f, 1.f, 0.f);
			v.vTangent = Vec3(1.f, 0.f, 0.f);
			v.vBinormal = Vec3(0.f, 0.f, -1.f);
	
			v.vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	
			vecVtx.push_back(v);
		}
	}
	
	// 인덱스
	for (UINT row = 0; row < m_iZFaceCount; ++row)
	{
		for (UINT col = 0; col < m_iXFaceCount; ++col)
		{
			// 0
			// | \
			// 2- 1
			vecIdx.push_back(row * (m_iXFaceCount + 1) + col + m_iXFaceCount + 1);
			vecIdx.push_back(row * (m_iXFaceCount + 1) + col + 1);
			vecIdx.push_back(row * (m_iXFaceCount + 1) + col);
			
			// 1- 2
			//  \ |
			//    0
			vecIdx.push_back(row * (m_iXFaceCount + 1) + col + 1);
			vecIdx.push_back(row * (m_iXFaceCount + 1) + col + m_iXFaceCount + 1);
			vecIdx.push_back(row * (m_iXFaceCount + 1) + col + m_iXFaceCount + 1 + 1);
		}
	}

	pMesh = new CMesh;
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	pMesh->SetKey(L"LandscapeMesh");

	tEvent evn = {};

	// 추가
	AddRes(pMesh, RES_TYPE::MESH);
	SetMesh(pMesh);
}

void CLandScape::CreateMaterial()
{
	if (nullptr != GetSharedMaterial())
		return;

	Ptr<CGraphicsShader> pShader = new CGraphicsShader;
	pShader->SetKey(L"LandScapeShader");
	pShader->CreateVertexShader(L"shader\\landscape.fx", "VS_LandScape");
	pShader->CreateHullShader(L"shader\\landscape.fx", "HS_LandScape");
	pShader->CreateDomainShader(L"shader\\landscape.fx", "DS_LandScape");
	pShader->CreatePixelShader(L"shader\\landscape.fx", "PS_LandScape");

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	pShader->SetRSType(RS_TYPE::CULL_BACK);
	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQUE);

	pShader->AddScalarParam(INT_0, "Tess");
	pShader->AddScalarParam(VEC4_0, "Camera Distance");
	pShader->AddTexureParam(TEX_0, "HeightMap");
	pShader->AddTexureParam(TEX_1, "texture\\MaterialAlb_Slice_18_.png");

	tEvent evn = {};

	// 추가
	AddRes(pShader.Get(), RES_TYPE::GRAPHICS_SHADER);

	// 재질
	Ptr<CMaterial> pMtrl = new CMaterial(true);
	pMtrl->SetShader(pShader);
	pMtrl->SetKey(L"LandScapeMtrl");
	pMtrl->SetTexParam(TEX_1,CResMgr::GetInst()->FindRes<CTexture>(L"texture\\MaterialAlb_Slice_18_.png"));
	SetSharedMaterial(pMtrl);

	// 추가
	AddRes(pMtrl.Get(), RES_TYPE::MATERIAL);

	// =====================
	// 지형 피킹 컴퓨트 쉐이더
	// =====================
	m_pCSRaycast = (CRaycastShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"RaycastShader").Get();
	if (nullptr == m_pCSRaycast)
	{
		m_pCSRaycast = new CRaycastShader();
		m_pCSRaycast->CreateComputeShader(L"shader\\raycast.fx", "CS_Raycast");
		m_pCSRaycast->SetKey(L"RaycastShader");
		AddRes(m_pCSRaycast.Get(), RES_TYPE::COMPUTE_SHADER);
	}

	// ======================
	// 높이 수정 컴퓨트 쉐이더
	// ======================
	m_pCSHeightMap = (CHeightMapShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"HeightMapShader").Get();
	if (nullptr == m_pHeightMap)
	{
		m_pCSHeightMap = new CHeightMapShader;
		m_pCSHeightMap->CreateComputeShader(L"shader\\heightmap.fx", "CS_HeightMap");
		m_pCSHeightMap->SetKey(L"HeightMapShader");
		AddRes(m_pCSHeightMap.Get(), RES_TYPE::COMPUTE_SHADER);
	}
}

void CLandScape::CreateTexture()
{
	// 높이맵 텍스쳐	

	m_pHeightMap = CResMgr::GetInst()->CreateTexture(L"HeightMap"
		, 2048, 2048
		, DXGI_FORMAT_R32_FLOAT
		, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);

	Ptr<CMaterial> pMtrl = GetSharedMaterial();
	pMtrl->SetScalarParam(SCALAR_PARAM::INT_0, &m_iXFaceCount);
	pMtrl->SetScalarParam(SCALAR_PARAM::INT_1, &m_iZFaceCount);

	Update_HeightMap();

	m_pBrushTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\brush\\Brush_02.png");

	m_bCreateTex = true;
}

void CLandScape::Initialize()
{
	// 높이맵 텍스쳐	

	if (m_bCreateTex)
	{
		DeleteRes(m_pHeightMap.Get(), RES_TYPE::TEXTURE);

		m_pHeightMap = CResMgr::GetInst()->CreateTexture(L"HeightMap1"
			, 2048, 2048
			, DXGI_FORMAT_R32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
	
		m_bCreateTex = false;
	}
	else
	{
		DeleteRes(m_pHeightMap.Get(), RES_TYPE::TEXTURE);

		m_pHeightMap = CResMgr::GetInst()->CreateTexture(L"HeightMap"
			, 2048, 2048
			, DXGI_FORMAT_R32_FLOAT
			, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);

		m_bCreateTex = true;
	}

	Update_HeightMap();


}

void CLandScape::SetHeightMap(Ptr<CTexture> _pTex)
{
	m_pHeightMap = _pTex;

	Update_HeightMap();
}

void CLandScape::Update_HeightMap()
{
	Ptr<CMaterial> pMtrl = GetSharedMaterial();
	Vec2 vResolution = Vec2(m_pHeightMap->GetWidth(), m_pHeightMap->GetHeight());
	pMtrl->SetScalarParam(SCALAR_PARAM::VEC2_0, &vResolution);
	pMtrl->SetTexParam(TEX_PARAM::TEX_0, m_pHeightMap);
	m_pCSRaycast->SetHeightMap(m_pHeightMap);
	m_pCSHeightMap->SetHeightMap(m_pHeightMap);
}