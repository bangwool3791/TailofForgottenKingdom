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
	, m_vBrushScale(Vec2(0.1f, 0.1f))
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
}

CLandScape::~CLandScape()
{
	if (nullptr != m_pCrossBuffer)
		delete m_pCrossBuffer;
}


void CLandScape::finaltick()
{
	Raycasting();

	// 픽킹 정보를 system memory 로 가져옴
	tRaycastOut out = {};
	m_pCrossBuffer->GetData(&out);

	/*
	* CS 레이 캐스트 결과 가져온다.
	*/
	if (KEY_PRESSED(KEY::LBTN))
	{
		// 교점 위치정보를 토대로 높이를 수정 함
		m_pCSHeightMap->SetInputBuffer(m_pCrossBuffer); // 픽킹 정보를 HeightMapShader 에 세팅
		m_pCSHeightMap->SetBrushTex(m_pBrushTex);		// 사용할 브러쉬 텍스쳐 세팅
		m_pCSHeightMap->SetBrushIndex(0);				// 브러쉬 인덱스 설정
		m_pCSHeightMap->SetBrushScale(m_vBrushScale);   // 브러쉬 크기
		m_pCSHeightMap->SetHeightMap(m_pHeightMap);
		m_pCSHeightMap->Execute();
	}

	if (KEY_PRESSED(KEY::S))
	{
		SaveTexture();
	}
}

#include "CLevelMgr.h"

void CLandScape::render()
{
	if (nullptr == GetMesh() || nullptr == GetCurMaterial())
		return;

	Transform()->UpdateData();
	
	GetCurMaterial()->SetScalarParam(INT_0, &m_iXFaceCount);
	GetCurMaterial()->SetScalarParam(INT_1, &m_iZFaceCount);

	GetCurMaterial()->UpdateData();
	GetMesh()->render();

	CMaterial::Clear();
}

void CLandScape::SetFaceCount(UINT _X, UINT _Z)
{
	m_iXFaceCount = _X;
	m_iZFaceCount = _Z;

	// 면 개수에 맞는 메시 생성
	CreateMesh();
}

void CLandScape::SaveTexture()
{
	m_pBrushTex->SaveTexture();
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
	tRaycastOut out = { Vec2(0.f, 0.f), 0x7fffffff, 0 };
	m_pCrossBuffer->SetData(&out, 1);

	m_pCSRaycast->SetHeightMap(m_pHeightMap);
	m_pCSRaycast->SetFaceCount(m_iXFaceCount, m_iZFaceCount);
	m_pCSRaycast->SetCameraRay(CamRay);
	m_pCSRaycast->SetOuputBuffer(m_pCrossBuffer);

	m_pCSRaycast->Execute();
}