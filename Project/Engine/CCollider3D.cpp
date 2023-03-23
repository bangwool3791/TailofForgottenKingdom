#include "pch.h"
#include "CCollider3D.h"
#include "CTransform.h"
#include "CScript.h"

CCollider3D::CCollider3D()
	: CCollider(COMPONENT_TYPE::COLLIDER3D)
	, m_vScale(Vec3(1.f, 1.f, 1.f))
	, m_eType(COLLIDER3D_TYPE::COLLIDER3D_CUBE)
{
	SetName(L"CCollider3D");
}

CCollider3D::~CCollider3D()
{
}

void CCollider3D::finaltick()
{
	// FinalPos 계산
	if (nullptr == Transform())
		return;

	Vec3 vObjectPos = Transform()->GetWorldPos();
	m_vFinalPos = m_vOffsetPos + vObjectPos;

	// 행렬 연산
	// 크 회 이
	m_vFinalScale = m_vScale;

	if (!m_bIgnorObjectScale)
	{
		m_vFinalScale *= Transform()->GetWorldScale();
	}

	Matrix matScale = XMMatrixScaling(m_vFinalScale.x, m_vFinalScale.y, m_vFinalScale.z);

	m_vRot *= Transform()->GetRelativeRotation();
	Matrix matRot = XMMatrixRotationX(m_vRot.x);
	matRot *= XMMatrixRotationY(m_vRot.y);
	matRot *= XMMatrixRotationZ(m_vRot.z);

	Matrix matTrans = XMMatrixTranslation(m_vFinalPos.x, m_vFinalPos.y, m_vFinalPos.z);

	// 충돌체 월드 행렬
	m_matWorld = matScale * matRot * matTrans;

	// DebugDraw 요청
#ifdef _DEBUG
	Vec4 Color = { 0.f, 0.f, 1.f, 1.f };
	if (0 < m_iOverlapCount)
		Color = Vec4{ 1.f, 0.f, 0.f, 1.f };

	if (COLLIDER3D_TYPE::COLLIDER3D_CUBE == m_eType)
	{
		DebugDrawCube(Color, m_vFinalPos, m_vFinalScale, m_vRot);
	}
	else
	{
		DebugDrawSphere(Color, m_vFinalPos, m_vFinalScale.x);
	}
#endif
}

void CCollider3D::SaveToFile(FILE* _File)
{
	COMPONENT_TYPE type = GetType();
	fwrite(&type, sizeof(UINT), 1, _File);

	//__super::SaveToFile(_File);
	
	fwrite(&m_vOffsetPos, sizeof(Vec3), 1, _File);
	fwrite(&m_vScale, sizeof(Vec3), 1, _File);
	fwrite(&m_eType, sizeof(UINT), 1, _File);
}

void CCollider3D::LoadFromFile(FILE* _File)
{
	//__super::LoadFromFile(_File);

	fread(&m_vOffsetPos, sizeof(Vec3), 1, _File);
	fread(&m_vScale, sizeof(Vec3), 1, _File);
	fread(&m_eType, sizeof(UINT), 1, _File);
}