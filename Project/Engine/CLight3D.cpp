#include "pch.h"
#include "CLight3D.h"

#include "CDevice.h"
#include "CRenderMgr.h"
#include "CTransform.h"
#include "CStructuredBuffer.h"
#include "CSLight.h"

CLight3D::CLight3D()
	: CComponent(COMPONENT_TYPE::LIGHT3D)
	, m_iLightIdx(-1)
{
}

CLight3D::~CLight3D()
{
}


void CLight3D::finaltick()
{
	if (LIGHT_TYPE::POINT == m_Info.iLightType || LIGHT_TYPE::POINT2 == m_Info.iLightType)
	{
		Transform()->SetRelativeScale(Vec3(m_Info.fRadius * 2.f, m_Info.fRadius * 2.f, m_Info.fRadius * 2.f));
	}

	m_Info.vWorldPos = Transform()->GetWorldPos();
	m_Info.vWorldDir = Transform()->GetWorldDir(DIR::FRONT);

	m_iLightIdx = CRenderMgr::GetInst()->RegisterLight3D(this);
}

void CLight3D::render()
{
	if (LIGHT_TYPE::POINT == m_Info.iLightType || LIGHT_TYPE::POINT2 == m_Info.iLightType)
		DebugDrawSphere(Vec4(0.2f, 0.8f, 0.2f, 1.f), Transform()->GetWorldPos(), Transform()->GetWorldScale().x * 0.5f);
	else if(LIGHT_TYPE::SPOT == m_Info.iLightType || LIGHT_TYPE::SPOT2 == m_Info.iLightType)
		DebugDrawCone(Vec4(0.2f, 0.8f, 0.2f, 1.f), Transform()->GetWorldPos(), Transform()->GetWorldScale(), Transform()->GetRelativeRotation(), 0.f);

	if (nullptr == m_pLightMtrl)
		return;

	// Transform Update
	Transform()->UpdateData();

	m_pLightMtrl->SetScalarParam(INT_0, &m_iLightIdx);
	// 재질 바인딩(쉐이더, 상수, 텍스쳐(타겟) 등등)
	m_pLightMtrl->UpdateData();

	// 렌더링
	m_pVolumeMesh->render();

	CMaterial::Clear();
}

void CLight3D::SetLightType(LIGHT_TYPE _eType)
{
	m_Info.iLightType = _eType;

	if (LIGHT_TYPE::DIRECTIONAL == m_Info.iLightType)
	{
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
		m_pLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DirLightMtrl");
	}
	else if(LIGHT_TYPE::POINT == m_Info.iLightType || LIGHT_TYPE::POINT2 == m_Info.iLightType)
	{
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"SphereMesh");
		m_pLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"PointLightMtrl");
	}
	else if (LIGHT_TYPE::SPOT == m_Info.iLightType || LIGHT_TYPE::SPOT2 == m_Info.iLightType)
	{
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"ConeMesh");
		m_pLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"SpotLightMtrl");
	}
	else
	{

	}
}

void CLight3D::SaveToFile(FILE* _File)
{
}

void CLight3D::LoadFromFile(FILE* _File)
{
}
