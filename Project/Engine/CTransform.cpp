#include "pch.h"
#include "CTransform.h"

#include "CCamera.h"

#include "CTimeMgr.h"
#include "CKeyMgr.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CDevice.h"
#include "CConstBuffer.h"

#include "CMeshRender.h"

#include "extern.cpp"

CTransform::CTransform()
	:CComponent{COMPONENT_TYPE::TRANSFORM}
	, m_vRelativeScale{1.f,1.f,1.f}
	, m_blgnParentScale(false)
{
	SetName(L"CTransform");

	m_vRelativeDir[(UINT)DIR::RIGHT]	= Vec3{ 1.0f, 0.0f, 0.0f };
	m_vRelativeDir[(UINT)DIR::UP]		= Vec3{ 0.0f, 1.0f, 0.0f };
	m_vRelativeDir[(UINT)DIR::FRONT]	= Vec3{ 0.0f, 0.0f, 1.0f };
}

CTransform::~CTransform()
{

}

void CTransform::tick()
{
}

void CTransform::finaltick()
{
	m_matWorld = XMMatrixIdentity();
	Matrix		matScail = XMMatrixScaling(m_vRelativeScale.x, m_vRelativeScale.y, m_vRelativeScale.z);

	Matrix		matPos = XMMatrixTranslation(m_vRelativePos.x, m_vRelativePos.y, m_vRelativePos.z);
	//Degree 단위로 저장 된 회전 x,y,z 정보를 행렬에 담는다.
	m_matRelativeRot = XMMatrixRotationX(m_vRelativeRotation.x);
	m_matRelativeRot *= XMMatrixRotationY(m_vRelativeRotation.y);
	m_matRelativeRot *= XMMatrixRotationZ(m_vRelativeRotation.z);

	//행렬에 담긴 x,y,z축에 대한 회전 정보(Vector3)를 담는다.
	m_vRelativeDir[(UINT)DIR::RIGHT] = m_vWorldDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3{1.f, 0.f, 0.f}, m_matRelativeRot);
	m_vRelativeDir[(UINT)DIR::UP] = m_vWorldDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3{ 0.f, 1.f, 0.f }, m_matRelativeRot);
	m_vRelativeDir[(UINT)DIR::FRONT] = m_vWorldDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3{ 0.f, 0.f, 1.f }, m_matRelativeRot);

	m_matWorld = matScail * m_matRelativeRot * matPos ;

	if (GetOwner()->Get_Parent())
	{
		if (m_blgnParentScale)
		{
			CGameObject* pParent = GetOwner()->Get_Parent();
			Vec3         vParentScale = Vec3(1.f, 1.f, 1.f);
			while (pParent)
			{
				vParentScale *= pParent->Transform()->GetRelativeScale();

				if (pParent->Transform()->m_blgnParentScale)
					pParent = nullptr;
				else
					pParent = pParent->Get_Parent();
			}

			if (0.f == vParentScale.x)
			{
				vParentScale.x = 1.f;
			}
			else if (0.f == vParentScale.y)
			{
				vParentScale.y = 1.f;
			}
			else if (0.f == vParentScale.z)
			{
				vParentScale.z = 1.f;
			}

			Matrix matParentScaleInv = XMMatrixScaling(vParentScale.x, vParentScale.y, vParentScale.z);
			matParentScaleInv		 = XMMatrixInverse(nullptr, matParentScaleInv);

			m_matWorld = m_matWorld * matParentScaleInv * GetOwner()->Get_Parent()->Transform()->GetWorldMat();
		}
		else
		{
			m_matWorld = m_matWorld * GetOwner()->Get_Parent()->Transform()->GetWorldMat();
		}

		//부모가 있을 경우, 스케일이 포함 된 월드의 right, up, look이 방향 벡터이므로, 노멀라이즈한다.
		m_vWorldDir[(UINT)DIR::RIGHT] = XMVector3TransformNormal(Vec3(1.f, 0.f, 0.f), m_matWorld);
		m_vWorldDir[(UINT)DIR::UP] = XMVector3TransformNormal(Vec3(0.f, 1.f, 0.f), m_matWorld);
		m_vWorldDir[(UINT)DIR::FRONT] = XMVector3TransformNormal(Vec3(0.f, 0.f, 1.f), m_matWorld);

		// 회전, 크기 변환이 이루어졌기 때문에 변경된 크기를 초기화 하기 위해 Normalize 해준다.
		m_vWorldDir[(UINT)DIR::RIGHT].Normalize();
		m_vWorldDir[(UINT)DIR::UP].Normalize();
		m_vWorldDir[(UINT)DIR::FRONT].Normalize();
	}

	m_matWorldInv = XMMatrixInverse(nullptr, m_matWorld);
}

void CTransform::Update()
{
	g_transform.matWorld = m_matWorld;
	g_transform.matWV = g_transform.matWorld * g_transform.matView;
	g_transform.matWVP = g_transform.matWV * g_transform.matProj;
}


void CTransform::UpdateData()
{
	CConstBuffer* pCB = CDevice::GetInst()->GetConstBuffer(CB_TYPE::TRANSFORM);
	
	g_transform.matWorld = m_matWorld;
	g_transform.matWorldInv = m_matWorldInv;
	g_transform.matWV	 = g_transform.matWorld * g_transform.matView;
	g_transform.matWVP	 = g_transform.matWV * g_transform.matProj;

	pCB->SetData(&g_transform);
	pCB->UpdateData(ALL_STAGE);
}

Vec3 CTransform::GetWorldScale()
{
	Vec3 vWorldScale = m_vRelativeScale;

	if (m_blgnParentScale)
		return m_vRelativeScale;

	CGameObject* pParentObject = GetOwner()->Get_Parent();

	while (pParentObject)
	{
		vWorldScale *= pParentObject->Transform()->GetRelativeScale();

		if (pParentObject->Transform()->m_blgnParentScale)
			pParentObject = nullptr;
		else
			pParentObject = pParentObject->Get_Parent();

	}
	return vWorldScale;
}

Matrix CTransform::GetWorldRotation()
{
	Matrix matWorldRot = m_matRelativeRot;

	CGameObject* pParent = GetOwner()->GetParent();

	while (pParent)
	{
		matWorldRot *= pParent->Transform()->m_matRelativeRot;
		pParent = pParent->GetParent();
	}

	return matWorldRot;
}

void CTransform::SaveToFile(FILE* _File)
{
	COMPONENT_TYPE eType = GetType();

	fwrite(&eType, sizeof(COMPONENT_TYPE), 1, _File);

	fwrite(&m_vRelativePos , sizeof(Vec3), 1, _File);
	fwrite(&m_vRelativeScale , sizeof(Vec3), 1, _File);
	fwrite(&m_vRelativeRotation , sizeof(Vec3), 1, _File);
	fwrite(&m_blgnParentScale , sizeof(bool), 1, _File);
}

void CTransform::LoadFromFile(FILE* _File)
{
	fread(&m_vRelativePos, sizeof(Vec3), 1, _File);
	fread(&m_vRelativeScale, sizeof(Vec3), 1, _File);
	fread(&m_vRelativeRotation, sizeof(Vec3), 1, _File);
	fread(&m_blgnParentScale, sizeof(bool), 1, _File);
}

bool CTransform::Picking(const Ray& _ray, Vec3& _vPos)
{
	float fDist = 0.f;

	m_vecPoint.clear();
	m_vecPoint.shrink_to_fit();

	if (nullptr == GetOwner()->MeshRender() || nullptr == GetOwner()->MeshRender()->GetMesh())
		return false;

	Ptr<CMesh> pMesh = GetOwner()->MeshRender()->GetMesh();
	Vec3 vResult{ -1.f, -1.f, -1.f };
	size_t verts;
	Vtx* vertices = pMesh->GetVertices(verts);

	finaltick();

	Vec3 vPos{};

	for (size_t i{}; i < verts; ++i)
	{
		vPos = XMVector3TransformCoord(vertices[i].vPos, m_matWorld);
		m_vecPoint.push_back(vPos);
	}

	for (UINT i = 0; i < verts; i += 3)
	{
		if (verts - i < 3)
			break;

		if (_ray.Intersects(m_vecPoint[i], m_vecPoint[i + 1], m_vecPoint[i + 2], fDist))
		{
			vResult = _ray.direction * fDist + _ray.position;
			_vPos = vResult;
			return true;
		}
	}

	return false;
}
