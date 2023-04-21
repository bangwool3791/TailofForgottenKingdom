#include "pch.h"
#include "PhysXComponent.h"
#include "PhysXMgr.h"

#include "CTransform.h"
#include "particle.h"

using namespace cyclone;

CPhysXComponent::CPhysXComponent()
	:CComponent(COMPONENT_TYPE::PHYSX)
	, AmmoRound{}
{
}

CPhysXComponent::CPhysXComponent(const CPhysXComponent& _rhs)
	:CComponent(COMPONENT_TYPE::PHYSX)
	, AmmoRound{}
	, m_ePhysType{ _rhs.m_ePhysType }
{
}


CPhysXComponent::~CPhysXComponent()
{
	if (AmmoRound)
	{
		AmmoRound->type = UNUSED;
		AmmoRound->idx = 0xffff;
	}
}

void CPhysXComponent::begin()
{

}

void CPhysXComponent::finaltick()
{
	if (nullptr == AmmoRound)
		return;

	if (AmmoRound->type != UNUSED)
	{
		PhysXVec3 vPhysPos = AmmoRound->particle.getPosition();

		Vec3 vPos = GetOwner()->Transform()->GetRelativePos();

		vPos.x -= vPhysPos.x;
		vPos.y += vPhysPos.y;
		vPos.z -= vPhysPos.z;

		GetOwner()->Transform()->SetRelativePos(vPos);
	}
}

void CPhysXComponent::SaveToFile(FILE* _File)
{
	COMPONENT_TYPE type = __super::GetType();
	fwrite(&type, sizeof(UINT), 1, _File);

	fwrite(&m_ePhysType, sizeof(PhysType), 1, _File);
}

void CPhysXComponent::LoadFromFile(FILE* _File)
{
	fread(&m_ePhysType, sizeof(PhysType), 1, _File);
}

bool CPhysXComponent::IsRelease()
{
	if (m_bRun && AmmoRound->type == UNUSED)
	{
		m_bRun = false;
		return true;
	}
	return false;
}