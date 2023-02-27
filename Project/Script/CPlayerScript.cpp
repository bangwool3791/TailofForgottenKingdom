#include "pch.h"
#include "CPlayerScript.h"

#include <Engine\CDevice.h>
#include <Engine\CLayer.h>
#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CCollisionMgr.h>
#include <Engine\CScript.h>
#include <Engine\CCollider.h>
#include <Engine\CTransform.h>
#include <Engine\CMaterial.h>
#include <Engine\CMeshRender.h>

#include <Engine\CScript.h>

CPlayerScript::CPlayerScript()
	:CScript{PLAYERSCRIPT}
	, m_fSpeed{100.f}
{
	SetName(L"CPlayerScript");

	AddScriptParam(SCRIPT_PARAM::FLOAT, "Player MoveSpeed", &m_fSpeed);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Player JumpHeight", &m_fJumpHeight);

	m_Prefab = CResMgr::GetInst()->FindRes<CPrefab>(L"ShadowPrefab");
}

CPlayerScript::~CPlayerScript()
{
}


void CPlayerScript::begin()
{
	//m_vTarget = GetOwner()->Transform()->GetRelativePos();
}

void CPlayerScript::tick()
{
	//Vec3 vPos = Transform()->GetRelativePos();
	//vPos.z = 1.f;

	///*
	//* Occur Frame Drop
	//*/
	//if ((m_vTarget - vPos).Length() > 1.f)
	//{
	//	auto vMove = m_vTarget - vPos;
	//	vPos += vMove * DT;
	//	Transform()->SetRelativePos(vPos);
	//}
}

void CPlayerScript::BeginOverlap(CCollider* _pOther)
{
}

void CPlayerScript::Overlap(CCollider* _pOther)
{
	//Vec3 vSource = GetOwner()->Transform()->GetRelativePos();
	//Vec3 vTarget = _pOther->GetOwner()->Transform()->GetRelativePos();
	//Vec3 vDir = vSource - vTarget;
	//vDir.Normalize();
	//vSource += (vSource - vTarget).Length() * vDir;
	//GetOwner()->Transform()->SetRelativePos(vSource);
}

void CPlayerScript::EndOverlap(CCollider* _pOther)
{
}

void CPlayerScript::SaveToFile(FILE* _File)
{
	CScript::SaveToFile(_File);
	fwrite(&m_fSpeed, sizeof(float), 1, _File);
	fwrite(&m_fJumpHeight, sizeof(float), 1, _File);

	SaveResourceRef(m_Prefab, _File);
}

void CPlayerScript::LoadFromFile(FILE* _File)
{
	CScript::LoadFromFile(_File);
	fread(&m_fSpeed, sizeof(float), 1, _File);
	fread(&m_fJumpHeight, sizeof(float), 1, _File);

	LoadResourceRef(m_Prefab, _File);
}
