#include "pch.h"
#include "CLandScape.h"

#include "CStructuredBuffer.h"

#include "CTransform.h"
#include "CCamera.h"

#include "CRenderMgr.h"
#include "CKeyMgr.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_iXFaceCount(1)
	, m_iZFaceCount(1)
	, m_vBrushScale(Vec2(0.3f, 0.3f))
	, m_pCrossBuffer(nullptr)
	, m_pWeightMapBuffer(nullptr)
	, m_iWeightWidth(0)
	, m_iWeightHeight(0)
	, m_iWeightIdx(0)
	, m_eMod(LANDSCAPE_MOD::NONE)

{
	SetFaceCount(1, 1);

	CreateMaterial();

	CreateTexture();

	/*
	* 마우스 피킹되는 지점 정보 받는 구조체 버퍼
	* 셰이더에서는 구조체 버퍼를 GPU - CPU 데이터 전달 수단
	*/
	m_pCrossBuffer = new CStructuredBuffer;
	m_pCrossBuffer->Create(sizeof(tRaycastOut), 1, SB_TYPE::UAV_INC, nullptr, true);

	// 타일 텍스쳐(Color, Normal 혼합, 총 6장)	
	m_pTileArrTex = CResMgr::GetInst()->Load<CTexture>(L"texture\\tile\\TILE_ARRR.dds", L"texture\\tile\\TILE_ARRR.dds");
	m_pTileArrTex->GenerateMip(8);

	//m_pTileArrTex = CResMgr::GetInst()->LoadTexture(L"TileArrayTex", L"texture\\tile\\TILE_ARRR.dds", 8);
}

CLandScape::~CLandScape()
{
	if (nullptr != m_pCrossBuffer)
		delete m_pCrossBuffer;

	if (nullptr != m_pWeightMapBuffer)
		delete m_pWeightMapBuffer;
}


void CLandScape::finaltick()
{
	Raycasting();

	if (LANDSCAPE_MOD::NONE == m_eMod)
	{
		return;
	}

	/*
	* CS 레이 캐스트 결과 가져온다.
	*/

	if (KEY_PRESSED(KEY::LBTN))
	{
		if (LANDSCAPE_MOD::HEIGHT_MAP == m_eMod)
		{
			// 픽킹 정보를 system memory 로 가져옴
			//tRaycastOut out = {};
			//m_pCrossBuffer->GetData(&out);

			// 교점 위치정보를 토대로 높이를 수정 함
			m_pCSHeightMap->SetInputBuffer(m_pCrossBuffer); // 픽킹 정보를 HeightMapShader 에 세팅
			m_pCSHeightMap->SetBrushTex(m_pBrushTex);		// 사용할 브러쉬 텍스쳐 세팅
			m_pCSHeightMap->SetBrushIndex(0);				// 브러쉬 인덱스 설정
			m_pCSHeightMap->SetBrushScale(m_vBrushScale);   // 브러쉬 크기
			m_pCSHeightMap->SetHeightMap(m_pHeightMap);
			m_pCSHeightMap->Execute();
		}
		else if (LANDSCAPE_MOD::SPLAT == m_eMod)
		{
			// 교점 위치정보를 가중치를 수정함	
			m_pCSWeightMap->SetInputBuffer(m_pCrossBuffer); // 레이 캐스트 위치
			m_pCSWeightMap->SetBrushArrTex(m_pBrushTex);
			m_pCSWeightMap->SetBrushIndex(0);
			m_pCSWeightMap->SetWeightClear(0.f);
			m_pCSWeightMap->SetBrushScale(m_vBrushScale); // 브러쉬 크기
			m_pCSWeightMap->SetWeightMap(m_pWeightMapBuffer, m_iWeightWidth, m_iWeightHeight); // 가중치맵, 가로 세로 개수			
			m_pCSWeightMap->SetWeightIdx(m_iWeightIdx);
			m_pCSWeightMap->Execute();
		}
		else if (LANDSCAPE_MOD::SPLAT_CLEAR == m_eMod)
		{
			// 교점 위치정보를 가중치를 수정함	
			m_pCSWeightMap->SetInputBuffer(m_pCrossBuffer); // 레이 캐스트 위치
			m_pCSWeightMap->SetBrushArrTex(m_pBrushTex);
			m_pCSWeightMap->SetBrushIndex(0);
			m_pCSWeightMap->SetWeightClear(1.f);
			m_pCSWeightMap->SetBrushScale(m_vBrushScale); // 브러쉬 크기
			m_pCSWeightMap->SetWeightMap(m_pWeightMapBuffer, m_iWeightWidth, m_iWeightHeight); // 가중치맵, 가로 세로 개수			
			m_pCSWeightMap->SetWeightIdx(m_iWeightIdx);
			m_pCSWeightMap->Execute();
		}
	}

	if (KEY_TAP(KEY::RIGHT))
	{
		if (LANDSCAPE_MOD::SPLAT == m_eMod)
		{
			m_iWeightIdx++;
			if (3 <= m_iWeightIdx)
				m_iWeightIdx = 0;
		}
	}
}

#include "CLevelMgr.h"

void CLandScape::render()
{
	if (nullptr == GetMesh() || nullptr == GetCurMaterial())
		return;

	Transform()->UpdateData();

	// 재질 정보 업데이트
	{
		// 가중치 버퍼 전달
		m_pWeightMapBuffer->UpdateData(17, PIPELINE_STAGE::PS);

		// 가중치 버퍼 해상도 전달
		Vec2 vWeightMapResolution = Vec2((float)m_iWeightWidth, (float)m_iWeightHeight);
		GetCurMaterial()->SetScalarParam(SCALAR_PARAM::VEC2_1, &vWeightMapResolution);

		// 지형 면 개수 전달
		GetCurMaterial()->SetScalarParam(INT_0, &m_iXFaceCount);
		GetCurMaterial()->SetScalarParam(INT_1, &m_iZFaceCount);

		// 타일 배열 개수 전달
		float m_fTileCount = float(m_pTileArrTex->GetArraySize() / 2); // 색상, 노말 합쳐져있어서 나누기 2 해줌
		GetCurMaterial()->SetScalarParam(SCALAR_PARAM::FLOAT_1, &m_fTileCount);

		Vec3 vPos = m_pCameraObj->Transform()->GetRelativePos();
		Vec4 temp = Vec4{ vPos, 1.f };
		GetCurMaterial()->SetScalarParam(SCALAR_PARAM::VEC4_0, &temp);

		// 타일 텍스쳐 전달
		GetCurMaterial()->SetTexParam(TEX_PARAM::TEX_ARR_0, m_pTileArrTex);

		// 업데이트
		GetCurMaterial()->UpdateData();
	}

	GetMesh()->render();

	// Clear
	CMaterial::Clear();
	m_pWeightMapBuffer->Clear();
}

void CLandScape::SetFaceCount(UINT _X, UINT _Z)
{
	m_iXFaceCount = _X;
	m_iZFaceCount = _Z;

	// 면 개수에 맞는 메시 생성
	CreateMesh();
}

void CLandScape::SaveTexture(const wstring& _path)
{
	m_pHeightMap->SaveTexture(_path);
}

void CLandScape::Raycasting()
{
	// 시점 카메라를 가져옴
	CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == pMainCam)
		return;

	// 월드 기준 광선을 지형의 로컬로 보냄
	const Matrix& matWorldInv = Transform()->GetWorldInvMat();
	const tRay& ray = pMainCam->GetRay();

	tRay CamRay = {};
	CamRay.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	CamRay.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	CamRay.vDir.Normalize();

	// 지형과 카메라 Ray 의 교점을 구함
	tRaycastOut out = { Vec2(0.f, 0.f), Vec3{0.f, 0.f, 0.f}, 0x7fffffff, 0 };
	m_pCrossBuffer->SetData(&out, 1);

	m_pCSRaycast->SetHeightMap(m_pHeightMap);
	m_pCSRaycast->SetFaceCount(m_iXFaceCount, m_iZFaceCount);
	m_pCSRaycast->SetCameraRay(CamRay);
	m_pCSRaycast->SetOuputBuffer(m_pCrossBuffer);

	m_pCSRaycast->Execute();
}

void CLandScape::SaveBmpFile(const wstring& path)
{
	m_pHeightMap->SaveBmpFile(path);
}

void CLandScape::LoadBmpFile(const wstring& path)
{
	m_pHeightMap->LoadBmpFile(path);
}

void CLandScape::SaveToFile(FILE* _File) 
{
	COMPONENT_TYPE type = GetType();
	fwrite(&type, sizeof(UINT), 1, _File);
	
	fwrite(&m_iXFaceCount, sizeof(UINT), 1, _File);
	fwrite(&m_iZFaceCount, sizeof(UINT), 1, _File);
	SaveResourceRef(m_pHeightMap, _File);
	SaveResourceRef(m_pTileArrTex, _File);

	UINT iElemCnt = m_pWeightMapBuffer->GetElementsCount();
	UINT iElemSize = m_pWeightMapBuffer->GetElementsSize();

	fwrite(&iElemCnt, sizeof(UINT), 1, _File);
	fwrite(&iElemSize, sizeof(UINT), 1, _File);

	tWeight_4* arr = new tWeight_4[iElemCnt];

	m_pWeightMapBuffer->GetData(arr, 0);

	for (size_t i = 0; i < iElemCnt; ++i)
		for(size_t j = 0; j < 4; ++j)
			fwrite(&arr[i].arrWeight[j], sizeof(float), 1, _File);

	delete arr;
	/*
	* m_pHeightMap
	* m_pTileArrTex
	* m_pCSWeightMap//m_pWeightMapBuffer
	* m_iWeightWidth//m_iWeightHeight
	*/
}

void CLandScape::LoadFromFile(FILE* _File)
{
	COMPONENT_TYPE type = GetType();
	fread(&type, sizeof(UINT), 1, _File);

	fread(&m_iXFaceCount, sizeof(UINT), 1, _File);
	fread(&m_iZFaceCount, sizeof(UINT), 1, _File);

	LoadResourceRef(m_pHeightMap, _File);
	Update_HeightMap();

	LoadResourceRef(m_pTileArrTex, _File);
	m_pTileArrTex->GenerateMip(8);

	UINT iElemCnt = 0;
	UINT iElemSize = 0;

	fread(&iElemCnt, sizeof(UINT), 1, _File);
	fread(&iElemSize, sizeof(UINT), 1, _File);

	tWeight_4* arr = new tWeight_4[iElemCnt];

	for (size_t i = 0; i < iElemCnt; ++i)
		for (size_t j = 0; j < 4; ++j)
			fread(&arr[i].arrWeight[j], sizeof(float), 1, _File);

	m_pWeightMapBuffer->SetData(arr, iElemCnt);

	delete arr;
}

const tRaycastOut& CLandScape::GetRay()
{
	return m_pCSRaycast->GetRay();
}

void CLandScape::SetTexArr(Ptr<CTexture> _ptr)
{
	m_pTileArrTex = _ptr;
	m_pTileArrTex->GenerateMip(8);
}
