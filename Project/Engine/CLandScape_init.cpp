#include "pch.h"
#include "CLandScape.h"

#include "CDevice.h"
#include "CResMgr.h"
#include "CEventMgr.h"

#include "CStructuredBuffer.h"

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

	pMesh = new CMesh(true);
	pMesh->Create(vecVtx.data(), (UINT)vecVtx.size(), vecIdx.data(), (UINT)vecIdx.size());
	pMesh->SetKey(L"LandscapeMesh");

	tEvent evn = {};

	// 추가
	AddRes(pMesh, RES_TYPE::MESH);
	SetMesh(pMesh);
	SetSharedMaterial(m_LandScapeMtrl, 0);
}

void CLandScape::CreateMaterial()
{
	if (nullptr != GetSharedMaterial(0))
		return;

	Ptr<CGraphicsShader> pShader = new CGraphicsShader;
	pShader->SetKey(L"LandScapeShader");
	pShader->CreateVertexShader(L"shader\\landscape.fx", "VS_LandScape");
	pShader->CreateHullShader(L"shader\\landscape.fx", "HS_LandScape");
	pShader->CreateDomainShader(L"shader\\landscape.fx", "DS_LandScape");
	pShader->CreatePixelShader(L"shader\\landscape.fx", "PS_LandScape");

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	pShader->SetBSType(BS_TYPE::DEFAULT);
	pShader->SetDSType(DS_TYPE::LESS);
	pShader->SetDomain(SHADER_DOMAIN::DOMAIN_DEFERRED_OPAQUE);

	pShader->AddScalarParam(INT_0, "Tess");
	pShader->AddScalarParam(VEC4_0, "Camera Distance");
	pShader->AddTexureParam(TEX_0, "HeightMap");

	tEvent evn = {};

	// 추가
	AddRes(pShader.Get(), RES_TYPE::GRAPHICS_SHADER);

	// 재질
	// 재질
	m_LandScapeMtrl = new CMaterial(true);
	m_LandScapeMtrl->SetShader(pShader);
	m_LandScapeMtrl->SetKey(L"LandScapeMtrl");
	SetSharedMaterial(m_LandScapeMtrl, 0);

	// 추가
	AddRes(m_LandScapeMtrl.Get(), RES_TYPE::MATERIAL);

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

	// =======================
	// 가중치 수정 컴퓨트 쉐이더
	// =======================
	m_pCSWeightMap = (CWeightMapShader*)CResMgr::GetInst()->FindRes<CComputeShader>(L"WeightMapShader").Get();
	if (nullptr == m_pCSWeightMap)
	{
		m_pCSWeightMap = new CWeightMapShader;
		m_pCSWeightMap->CreateComputeShader(L"shader\\weightmap.fx", "CS_WeightMap");
		AddRes(m_pCSWeightMap.Get(), RES_TYPE::COMPUTE_SHADER);
	}
}

void CLandScape::CreateTexture()
{
	// 높이맵 텍스쳐	

	m_pHeightMap = CResMgr::GetInst()->CreateTexture(L"HeightMap"
		, 2048, 2048
		, DXGI_FORMAT_R32_FLOAT
		, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);

	Ptr<CMaterial> pMtrl = GetSharedMaterial(0);
	pMtrl->SetScalarParam(SCALAR_PARAM::INT_0, &m_iXFaceCount);
	pMtrl->SetScalarParam(SCALAR_PARAM::INT_1, &m_iZFaceCount);

	Update_HeightMap();

	m_pBrushTex = CResMgr::GetInst()->FindRes<CTexture>(L"texture\\brush\\Brush_01.png");

	// 가중치 버퍼
	m_iWeightWidth = 1024;
	m_iWeightHeight = 1024;

	m_pWeightMapBuffer = new CStructuredBuffer;
	m_pWeightMapBuffer->Create(sizeof(tWeight_4), m_iWeightWidth * m_iWeightHeight, SB_TYPE::UAV_INC, nullptr, true);
}

void CLandScape::Initialize()
{
	// 높이맵 텍스쳐	

	//if (m_bCreateTex)
	//{
	//	DeleteRes(m_pHeightMap.Get(), RES_TYPE::TEXTURE);

	//	m_pHeightMap = CResMgr::GetInst()->CreateTexture(L"HeightMap1"
	//		, 2048, 2048
	//		, DXGI_FORMAT_R32_FLOAT
	//		, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);
	//
	//	m_bCreateTex = false;
	//}
	//else
	//{
	//	DeleteRes(m_pHeightMap.Get(), RES_TYPE::TEXTURE);

	//	m_pHeightMap = CResMgr::GetInst()->CreateTexture(L"HeightMap"
	//		, 2048, 2048
	//		, DXGI_FORMAT_R32_FLOAT
	//		, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);

	//	m_bCreateTex = true;
	//}

	ComPtr<ID3D11Texture2D> tex = m_pHeightMap->GetTex2D();
	D3D11_TEXTURE2D_DESC desc;
	tex->GetDesc(&desc);

	ScratchImage image;
	CaptureTexture(DEVICE, CONTEXT, tex.Get(), image);

	for (size_t i = 0; i < image.GetPixelsSize(); ++i)
	{
		image.GetPixels()[i] = 0;
	}

	// 원본데이터(밉맵 레벨 0) 를 각 칸에 옮긴다.   
	for (int i = 0; i < desc.ArraySize; ++i)
	{
		// GPU 에 데이터 옮기기(밉맵 포함)
		UINT iSubresIdx = D3D11CalcSubresource(0, i, desc.MipLevels);

		CONTEXT->UpdateSubresource(tex.Get(), iSubresIdx, nullptr
			, image.GetImage(0, i, 0)->pixels
			, image.GetImage(0, i, 0)->rowPitch
			, image.GetImage(0, i, 0)->slicePitch);
	}

	Update_HeightMap();
}

void CLandScape::SetHeightmapOffset(float _offset)
{
	static float foffset = 0;

	ComPtr<ID3D11Texture2D> tex = m_pHeightMap->GetTex2D();
	D3D11_TEXTURE2D_DESC desc;
	tex->GetDesc(&desc);

	ScratchImage image;
	CaptureTexture(DEVICE, CONTEXT, tex.Get(), image);

	for (size_t i = 0; i < image.GetPixelsSize(); ++i)
	{
		image.GetPixels()[i] += (_offset - foffset);
	}

	foffset = _offset;

	// 원본데이터(밉맵 레벨 0) 를 각 칸에 옮긴다.   
	for (int i = 0; i < desc.ArraySize; ++i)
	{
		// GPU 에 데이터 옮기기(밉맵 포함)
		UINT iSubresIdx = D3D11CalcSubresource(0, i, desc.MipLevels);

		CONTEXT->UpdateSubresource(tex.Get(), iSubresIdx, nullptr
			, image.GetImage(0, i, 0)->pixels
			, image.GetImage(0, i, 0)->rowPitch
			, image.GetImage(0, i, 0)->slicePitch);
	}

	Update_HeightMap();
}

void CLandScape::SetHeightMap(Ptr<CTexture> _pTex)
{
	m_pHeightMap = _pTex;

	Update_HeightMap();
}

void CLandScape::SetBrushMap(Ptr<CTexture> _pTex)
{
	m_pBrushTex = _pTex;

	m_pCSHeightMap->SetBrushTex(m_pBrushTex);
}

void CLandScape::Update_HeightMap()
{
	Ptr<CMaterial> pMtrl = GetSharedMaterial(0);
	Vec2 vResolution = Vec2(m_pHeightMap->GetWidth(), m_pHeightMap->GetHeight());
	pMtrl->SetScalarParam(SCALAR_PARAM::VEC2_0, &vResolution);
	pMtrl->SetTexParam(TEX_PARAM::TEX_0, m_pHeightMap);
	m_pCSRaycast->SetHeightMap(m_pHeightMap);
	m_pCSHeightMap->SetHeightMap(m_pHeightMap);
}