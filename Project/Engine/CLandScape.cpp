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
	* ���콺 ��ŷ�Ǵ� ���� ���� �޴� ����ü ����
	* ���̴������� ����ü ���۸� GPU - CPU ������ ���� ����
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

	// ��ŷ ������ system memory �� ������
	tRaycastOut out = {};
	m_pCrossBuffer->GetData(&out);

	/*
	* CS ���� ĳ��Ʈ ��� �����´�.
	*/
	if (KEY_PRESSED(KEY::LBTN))
	{
		// ���� ��ġ������ ���� ���̸� ���� ��
		m_pCSHeightMap->SetInputBuffer(m_pCrossBuffer); // ��ŷ ������ HeightMapShader �� ����
		m_pCSHeightMap->SetBrushTex(m_pBrushTex);		// ����� �귯�� �ؽ��� ����
		m_pCSHeightMap->SetBrushIndex(0);				// �귯�� �ε��� ����
		m_pCSHeightMap->SetBrushScale(m_vBrushScale);   // �귯�� ũ��
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

	// �� ������ �´� �޽� ����
	CreateMesh();
}

void CLandScape::SaveTexture()
{
	m_pBrushTex->SaveTexture();
}

void CLandScape::Raycasting()
{
	// ���� ī�޶� ������
	CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCam();
	if (nullptr == pMainCam)
		return;

	// ���� ���� ������ ������ ���÷� ����
	const Matrix& matWorldInv = Transform()->GetWorldInvMat();
	const tRay& ray = pMainCam->GetRay();

	tRay CamRay = {};
	CamRay.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	CamRay.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	CamRay.vDir.Normalize();

	// ������ ī�޶� Ray �� ������ ����
	tRaycastOut out = { Vec2(0.f, 0.f), 0x7fffffff, 0 };
	m_pCrossBuffer->SetData(&out, 1);

	m_pCSRaycast->SetHeightMap(m_pHeightMap);
	m_pCSRaycast->SetFaceCount(m_iXFaceCount, m_iZFaceCount);
	m_pCSRaycast->SetCameraRay(CamRay);
	m_pCSRaycast->SetOuputBuffer(m_pCrossBuffer);

	m_pCSRaycast->Execute();
}