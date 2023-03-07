#include "pch.h"
#include "CLandScape.h"

#include "CTransform.h"
#include "CCamera.h"

CLandScape::CLandScape()
	: CRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_iXFaceCount(1)
	, m_iZFaceCount(1)
{
	SetFaceCount(1, 1);
	CreateMaterial();

	int iDivide = 1;
	GetCurMaterial()->SetScalarParam(INT_0, &iDivide);
	GetCurMaterial()->SetTexParam(TEX_0, CResMgr::GetInst()->FindRes<CTexture>(L"texture\\landscape\\HeightMap_01.jpg"));
}

CLandScape::~CLandScape()
{
}


void CLandScape::finaltick()
{
}

#include "CLevelMgr.h"

void CLandScape::render()
{
	if (nullptr == GetMesh() || nullptr == GetCurMaterial())
		return;

	CGameObject* pObj = CLevelMgr::GetInst()->FindObjectByName(L"SecendCamera");

	GetCurMaterial()->SetScalarParam(VEC4_0, Vec4{ pObj->Transform()->GetRelativePos(), 1.f });

	Transform()->UpdateData();

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