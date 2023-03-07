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

	// 정점 배치
	//for (UINT row = 0; row < m_iZFaceCount + 1; ++row)
	//{
	//	for (UINT col = 0; col < m_iXFaceCount + 1; ++col)
	//	{
	//		v.vPos = Vec3((float)col, 0.f, (float)row);
	//		v.vUV = Vec2(col, m_iZFaceCount - row);
	//
	//		v.vNormal = Vec3(0.f, 1.f, 0.f);
	//		v.vTangent = Vec3(1.f, 0.f, 0.f);
	//		v.vBinormal = Vec3(0.f, 0.f, -1.f);
	//
	//		v.vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	//
	//		vecVtx.push_back(v);
	//	}
	//}
	//
	//// 인덱스
	//for (UINT row = 0; row < m_iZFaceCount; ++row)
	//{
	//	for (UINT col = 0; col < m_iXFaceCount; ++col)
	//	{
	//		// 0
	//		// | \
	//		// 2- 1
	//		vecIdx.push_back(row * (m_iXFaceCount + 1) + col + m_iXFaceCount + 1);
	//		vecIdx.push_back(row * (m_iXFaceCount + 1) + col + 1);
	//		vecIdx.push_back(row * (m_iXFaceCount + 1) + col);
	//		
	//		// 1- 2
	//		//  \ |
	//		//    0
	//		vecIdx.push_back(row * (m_iXFaceCount + 1) + col + 1);
	//		vecIdx.push_back(row * (m_iXFaceCount + 1) + col + m_iXFaceCount + 1);
	//		vecIdx.push_back(row * (m_iXFaceCount + 1) + col + m_iXFaceCount + 1 + 1);
	//
	//		// 0 - 1
	//		// |   |
	//		// 3 - 2
	//		//vecIdx.push_back(row * (m_iXFaceCount + 1) + col + m_iXFaceCount + 1);
	//		//vecIdx.push_back(row * (m_iXFaceCount + 1) + col + m_iXFaceCount + 1 + 1);
	//		//vecIdx.push_back(row * (m_iXFaceCount + 1) + col + 1 + 1);
	//		//vecIdx.push_back(row * (m_iXFaceCount + 1) + col + 1);
	//		
	//	}
	//}

	v.vPos = Vec3((float)-10.f, 0.f, (float)+10.f);
	v.vNormal = Vec3(0.f, 1.f, 0.f);
	v.vTangent = Vec3(1.f, 0.f, 0.f);
	v.vBinormal = Vec3(0.f, 0.f, -1.f);
	v.vColor = Vec4(1.f, 0.f, 1.f, 1.f);
	vecVtx.push_back(v);
	
	v.vPos = Vec3((float)+10.f, 0.f, (float)+10.f);
	vecVtx.push_back(v);
	v.vPos = Vec3((float)+10.f, 0.f, (float)-10.f);
	vecVtx.push_back(v);
	v.vPos = Vec3((float)-10.f, 0.f, (float)-10.f);
	vecVtx.push_back(v);

	vecIdx.push_back(0);
	vecIdx.push_back(1);
	vecIdx.push_back(2);

	vecIdx.push_back(3);
	//vecIdx.push_back(4);
	//vecIdx.push_back(5);

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

	pShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	pShader->SetRSType(RS_TYPE::WIRE_FRAME);
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
	Ptr<CMaterial> pMtrl = new CMaterial(true);
	pMtrl->SetShader(pShader);
	pMtrl->SetKey(L"LandScapeMtrl");
	SetSharedMaterial(pMtrl);

	// 추가
	AddRes(pMtrl.Get(), RES_TYPE::MATERIAL);
}