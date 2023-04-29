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
#include <Engine\PhysXComponent.h>
#include <Engine\PhysXMgr.h>
#include <Engine\CScript.h>

CPlayerScript::CPlayerScript()
	:CScript{PLAYERSCRIPT}
	, m_fSpeed{500.f}
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
	Vec3 vPos = Transform()->GetRelativePos();

	m_vecAnimation.push_back(GetOwner()->Animator3D());

	const vector<CGameObject*> vecChilds = GetOwner()->GetChilds();
	for (UINT i = 0; i < vecChilds.size(); ++i)
		m_vecAnimation.push_back(vecChilds[i]->Animator3D());

	m_pTriangleMesh = CResMgr::GetInst()->FindRes<CMesh>(L"AreneStage_Navi_Mesh");
	if(m_pTriangleMesh->IsNavValid(vPos))
		Transform()->SetRelativePos(vPos);

	Transform()->SetRelativeRotationX(0.f);
//	Transform()->SetRelativeRotationY(XM_PI);

	Set_Animation_Key(L"idle");

	PhysXComponent()->SetName(L"CPhysXComponent");
}

void CPlayerScript::tick()
{
	if(!m_bAttack && !m_bJump)
		Move();

	//IsAnimationEnd에 걸린 경우 m_bAttack true 이므로 false 초기화
	//!m_bAttack Move에서 애니메이션 방복하고, 이전 프레임에 공격이 끝났었음
	if ((!m_bJump) || (IsAnimationEnd() && !m_bJump))
	{
		m_bAttack = false;

		if (KEY_TAP(KEY::LBTN))
		{
			m_bAttack = true;

			if (0 == m_iLButtonCount)
			{
				m_iLButtonCount = 1;
				Set_Animation_Key(L"ciritic-fight-1");
				Set_Animation_Time(0.7f);

			}
			else if (1 == m_iLButtonCount)
			{
				m_iLButtonCount = 2;
				Set_Animation_Key(L"ciritic-fight-2");
				Set_Animation_Time(0.7f);
			}
			else if (2 == m_iLButtonCount)
			{
				m_iLButtonCount = 0;
				Set_Animation_Key(L"ciritic-fight-3");
				Set_Animation_Time(1.f);
			}
		}
	}

	if (!m_bJump)
	{
		if (KEY_TAP(KEY::SPACE))
		{
			m_bJump = true;
			Vec3 vPos = Transform()->GetRelativePos();
			vPos.y += 5.f;
			Transform()->SetRelativePos(vPos);
			Set_Animation_Key(L"jump-land");
			Set_Animation_Time(0.5f);
			PhysXMgr::GetInst()->add(GetOwner()->PhysXComponent());
		}
	}

	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();

		Vec3 vRot = Transform()->GetRelativeRotation();

		//vRot.x += -vMouseDir.y * DT * XM_PI;
		vRot.y += vMouseDir.x * DT * XM_PI;

		Transform()->SetRelativeRotation(vRot);
	}
}

void CPlayerScript::finaltick()
{
	Vec3 vPos = Transform()->GetRelativePos();
	//플레이어 위치 Nav 보정
	if (vPos.y < 0)
	{
		//수정 필요
 		if (m_pTriangleMesh->IsNavValid(vPos))
			Transform()->SetRelativePos(vPos);
	}

	if (PhysXComponent()->IsRelease())
	{
		m_bJump = false;
		Set_Animation_Key(L"idle");
		Set_Animation_Time(1.f);
	}
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

void CPlayerScript::Set_Animation_Time(float _fTime)
{
	for (UINT i = 0; i < m_vecAnimation.size(); ++i)
		m_vecAnimation[i]->SetTimeScale(_fTime);
}
bool CPlayerScript::IsAnimationEnd()
{
	if (GetOwner()->Animator3D()->IsEnd())
	{
		Set_Animation_Key(L"idle");
		return true;
	}

	return false;
}

void CPlayerScript::Move()
{
	static UINT iMoveCount = 0;

	Vec3 vPos = Transform()->GetRelativePos();

	Vec3 vFront = Transform()->GetRelativeDir(DIR::FRONT);
	Vec3 vRight = Transform()->GetRelativeDir(DIR::RIGHT);

	bool bCheck = false;

	if (KEY_TAP(KEY::W))
	{
		if (0 == iMoveCount)
		{
			iMoveCount = 1;
			Set_Animation_Key(L"walk");
		}
		else if (1 == iMoveCount)
		{
			iMoveCount = 2;
			Set_Animation_Key(L"run");
		}
	}

	if (KEY_PRESSED(KEY::W))
	{
		if (1 == iMoveCount)
		{
			vPos += DT * vFront * -m_fSpeed * 0.7;
			Set_Animation_Key(L"walk");
		}
		else if (2 == iMoveCount)
		{
			vPos += DT * vFront * -m_fSpeed * 1.2;
			Set_Animation_Key(L"run");
		}

		if (m_pTriangleMesh->IsNavValid(vPos))
			Transform()->SetRelativePos(vPos);

		if (KEY_PRESSED(KEY::A))
		{
			if (1 == iMoveCount)
			{
				vPos += DT * vFront * -m_fSpeed * 0.5f;
				vPos += DT * vRight * +m_fSpeed * 0.5f;
			}
			else if (2 == iMoveCount)
			{
				vPos += DT * vFront * -m_fSpeed * 0.75f;
				vPos += DT * vRight * +m_fSpeed * 0.75f;
			}

			if (m_pTriangleMesh->IsNavValid(vPos))
				Transform()->SetRelativePos(vPos);
		}

		if (KEY_PRESSED(KEY::D))
		{
			if (1 == iMoveCount)
			{

				vPos += DT * vFront * -m_fSpeed * 0.5f;
				vPos += DT * vRight * -m_fSpeed * 0.5f;
			}
			else if (2 == iMoveCount)
			{

				vPos += DT * vFront * -m_fSpeed * 0.75f;
				vPos += DT * vRight * -m_fSpeed * 0.75f;
			}

			if (m_pTriangleMesh->IsNavValid(vPos))
				Transform()->SetRelativePos(vPos);
		}
	}
	else if (KEY_PRESSED(KEY::S))
	{
		vPos += DT * vFront * +m_fSpeed;
		if (m_pTriangleMesh->IsNavValid(vPos))
			Transform()->SetRelativePos(vPos);

		Set_Animation_Key(L"walk-back");

		if (KEY_PRESSED(KEY::A))
		{
			vPos += DT * vFront * +m_fSpeed * 0.5f;
			vPos += DT * vRight * +m_fSpeed * 0.5f;

			if (m_pTriangleMesh->IsNavValid(vPos))
				Transform()->SetRelativePos(vPos);
		}

		if (KEY_PRESSED(KEY::D))
		{
			vPos += DT * vFront * +m_fSpeed * 0.5f;
			vPos += DT * vRight * -m_fSpeed * 0.5f;

			if (m_pTriangleMesh->IsNavValid(vPos))
				Transform()->SetRelativePos(vPos);
		}
	}
	else if (KEY_PRESSED(KEY::A))
	{
		vPos += DT * +vRight * m_fSpeed;
		if (m_pTriangleMesh->IsNavValid(vPos))
			Transform()->SetRelativePos(vPos);

		Set_Animation_Key(L"walk-left");
	}
	else if (KEY_PRESSED(KEY::D))
		{
			vPos += DT * -vRight * m_fSpeed;
			if (m_pTriangleMesh->IsNavValid(vPos))
				Transform()->SetRelativePos(vPos);

			Set_Animation_Key(L"walk-right");
		}

	if (!m_bJump && IsAnimationEnd())
	{
		iMoveCount = 0;
	}
}

void CPlayerScript::Test()
{

}