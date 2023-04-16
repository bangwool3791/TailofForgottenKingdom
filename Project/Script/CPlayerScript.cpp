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
	m_vecAnimation.push_back(GetOwner()->Animator3D());

	const vector<CGameObject*> vecChilds = GetOwner()->GetChilds();
	for (UINT i = 0; i < vecChilds.size(); ++i)
		m_vecAnimation.push_back(vecChilds[i]->Animator3D());

	m_pTriangleMesh = CResMgr::GetInst()->FindRes<CMesh>(L"TriangleMesh");

	m_vPos = Vec3{ 200.f, 0.f, 200.f };
}

void CPlayerScript::tick()
{
	if (KEY_PRESSED(KEY::W))
	{
		Vec3 vPos = m_vPos;
		vPos.z += DT * m_fSpeed;

		if (m_pTriangleMesh->IsNaviValid(vPos))
			m_vPos = vPos;

		Set_Animation_Key(L"walk");
	}
	else if (KEY_PRESSED(KEY::S))
	{
		Vec3 vPos = m_vPos;
		vPos.z -= DT * m_fSpeed;

		if (m_pTriangleMesh->IsNaviValid(vPos))
			m_vPos = vPos;

		Set_Animation_Key(L"walk-back");
	}
	else if (KEY_PRESSED(KEY::A))
	{
		Vec3 vPos = m_vPos;
		vPos.x -= DT * m_fSpeed;

		if (m_pTriangleMesh->IsNaviValid(vPos))
			m_vPos = vPos;

		Set_Animation_Key(L"walk-left");
	}
	else if (KEY_PRESSED(KEY::D))
	{
		Vec3 vPos = m_vPos;
		vPos.x += DT * m_fSpeed;

		if (m_pTriangleMesh->IsNaviValid(vPos))
			m_vPos = vPos;

		Set_Animation_Key(L"walk-right");
	}

	Transform()->SetRelativePos(m_vPos);
}

void CPlayerScript::BeginOverlap(CCollider* _pOther)
{
}

void CPlayerScript::Overlap(CCollider* _pOther)
{

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

void CPlayerScript::Set_Animation_Key(const wstring& _strKey)
{
	for (UINT i = 0; i < m_vecAnimation.size(); ++i)
		m_vecAnimation[i]->SetCurFrameKey(_strKey);
}
