#include "pch.h"
#include "CCollider.h"
#include "CTransform.h"
#include "CScript.h"

CCollider::CCollider(COMPONENT_TYPE _type)
	: CComponent(_type)
	, m_bIgnorObjectScale(false)
	, m_bPause{ false }
{
	SetName(L"CCollider");
}

CCollider::~CCollider()
{
}

void CCollider::finaltick()
{
}

void CCollider::BeginOverlap(CCollider* _pOther)
{
	++m_iOverlapCount;
	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (auto iter{ vecScripts.begin() }; iter != vecScripts.end(); ++iter)
	{
		(*iter)->BeginOverlap(_pOther);
	}
}

void CCollider::Overlap(CCollider* _pOther)
{
	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (auto iter{ vecScripts.begin() }; iter != vecScripts.end(); ++iter)
	{
		(*iter)->Overlap(_pOther);
	}
}

void CCollider::EndOverlap(CCollider* _pOther)
{
	--m_iOverlapCount;
	const vector<CScript*>& vecScripts = GetOwner()->GetScripts();
	for (auto iter{ vecScripts.begin() }; iter != vecScripts.end(); ++iter)
	{
		(*iter)->EndOverlap(_pOther);
	}
}

void  CCollider::SetRotation(Vec3 vRot)
{
	m_vRot = vRot;
}

void CCollider::SaveToFile(FILE* _File)
{
	COMPONENT_TYPE type = GetType();
	fwrite(&type, sizeof(UINT), 1, _File);

	fwrite(&m_vRot, sizeof(Vec3), 1, _File);
	fwrite(&m_bIgnorObjectScale, sizeof(bool), 1, _File);
}

void CCollider::LoadFromFile(FILE* _File)
{
	fread(&m_vRot, sizeof(Vec3), 1, _File);
	fread(&m_bIgnorObjectScale, sizeof(bool), 1, _File);
}