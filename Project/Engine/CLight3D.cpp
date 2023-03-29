#include "pch.h"
#include "CLight3D.h"

#include "CDevice.h"
#include "CRenderMgr.h"
#include "CCamera.h"
#include "CTransform.h"
#include "CStructuredBuffer.h"
#include "CSLight.h"


CLight3D::CLight3D()
	: CComponent(COMPONENT_TYPE::LIGHT3D)
	, m_iLightIdx(-1)
	, m_pLightCam{}
{
	m_pLightCam = new CGameObject;
	m_pLightCam->AddComponent(new CTransform);
	m_pLightCam->AddComponent(new CCamera);
}

CLight3D::CLight3D(const CLight3D& _origin)
	: CComponent(_origin)
	, m_Info(_origin.m_Info)
	, m_iLightIdx(-1)
	, m_pLightCam(nullptr)
{
	m_pLightCam = _origin.m_pLightCam->Clone();
}

CLight3D::~CLight3D()
{
	if (nullptr != m_pLightCam)
		delete m_pLightCam;
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

	// 광원의 카메라도 광원과 동일한 Transform 이 되도록 업데이트 한다.
	m_pLightCam->Transform()->SetRelativePos(Transform()->GetWorldPos());
	m_pLightCam->Transform()->SetRelativeRotation(DecomposeRotMat(Transform()->GetWorldRotation()));
	m_pLightCam->finaltick_module();
}

void CLight3D::render_depthmap()
{
	m_pLightCam->Camera()->SortShadowObject();
	m_pLightCam->Camera()->render_depthmap();
}

void CLight3D::render_depthmap(const vector<CGameObject*>& obj)
{
	m_pLightCam->Camera()->SortShadowObject(obj);
	m_pLightCam->Camera()->render_depthmap();
}

void CLight3D::SetLightDirection(Vec3 _vDir)
{
	m_Info.vWorldDir = _vDir;
	m_Info.vWorldDir.Normalize();

	Vec3 vFront = m_Info.vWorldDir;
	Vec3 vUp = Vec3(0.f, 1.f, 0.f);
	Vec3 vRight = XMVector3Cross(vUp, vFront);
	vRight.Normalize();
	vUp = XMVector3Cross(vFront, vRight);
	vUp.Normalize();

	Matrix matRot = XMMatrixIdentity();
	matRot._11 = vRight.x;	matRot._12 = vRight.y;	matRot._13 = vRight.z;
	matRot._21 = vUp.x;		matRot._22 = vUp.y;		matRot._23 = vUp.z;
	matRot._31 = vFront.x;	matRot._32 = vFront.y;	matRot._33 = vFront.z;

	Vec3 vRot = DecomposeRotMat(matRot);

	// 광원이 가리키는 방향이 Transform 의 Front 가 되도록 회전시켜준다.
	Transform()->SetRelativeRotation(vRot);
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

	// 방향성 광원인 경우 그림자 처리를 위해서 광원카메라로 투영시킬 수 있게 View * Proj 행렬을 전달
	if (LIGHT_TYPE::DIRECTIONAL == (LIGHT_TYPE)m_Info.iLightType)
	{
		Matrix matLightVP = m_pLightCam->Camera()->GetViewMat() * m_pLightCam->Camera()->GetProjMat();
		m_pLightMtrl->SetScalarParam(SCALAR_PARAM::MAT_0, &matLightVP);
		m_pLightMtrl->SetTexParam(TEX_PARAM::TEX_3, CResMgr::GetInst()->FindRes<CTexture>(L"DepthMapTex"));
	}

	// 렌더링
	m_pVolumeMesh->render(0);

	CMaterial::Clear();
}

void CLight3D::SetLightType(LIGHT_TYPE _eType)
{
	m_Info.iLightType = _eType;

	if (LIGHT_TYPE::DIRECTIONAL == m_Info.iLightType)
	{
		m_pVolumeMesh = CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh");
		m_pLightMtrl = CResMgr::GetInst()->FindRes<CMaterial>(L"DirLightMtrl");

		m_pLightCam->Camera()->SetProjType(PROJ_TYPE::ORTHOGRAHPIC);
		m_pLightCam->Camera()->SetWidth(4096.f);
		m_pLightCam->Camera()->SetAspectRatio(1.f);
		m_pLightCam->Camera()->SetFar(100000.f);
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
