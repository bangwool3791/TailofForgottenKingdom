#include "pch.h"
#include "CPlayerTestScript.h"

#include "CEditor.h"
#include "CGameObjectEx.h"

#include <Engine\CDevice.h>
#include <Engine\CLayer.h>
#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>

#include <Engine\CCollisionMgr.h>
#include <Engine\CScript.h>
#include <Engine\CCollider.h>
#include <Engine\CTrailComponent.h>
#include <Engine\CTransform.h>
#include <Engine\CMaterial.h>
#include <Engine\CMeshRender.h>
#include <Engine\PhysXComponent.h>
#include <Engine\PhysXMgr.h>
#include <Engine\CScript.h>

CPlayerTestScript::CPlayerTestScript()
	:CScript{ -1 }
	, m_fSpeed{ 500.f }
	, m_fSworTrailSpeed{1.f}
{
	SetName(L"CPlayerTestScript");

	AddScriptParam(SCRIPT_PARAM::FLOAT, "Player MoveSpeed", &m_fSpeed);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Sword Trail Speed Inv", &m_fSworTrailSpeed);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Player JumpHeight", &m_fJumpHeight);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "Player AttackDelay", &m_fAttackDelay);

	m_Prefab = CResMgr::GetInst()->FindRes<CPrefab>(L"ShadowPrefab");
}

CPlayerTestScript::~CPlayerTestScript()
{
}


void CPlayerTestScript::begin()
{
	Vec3 vPos = Transform()->GetRelativePos();

	TrailComponent()->SetBoneSocketBuffer(Animator3D()->GetBoneSocketMat());

	m_pUpperSwordObj = CEditor::GetInst()->FindByName(L"SwordTrailUppder");
	m_pBottomSwordObj = CEditor::GetInst()->FindByName(L"SwordTrailBottom");
	m_pSworTrailObj = CEditor::GetInst()->FindByName(L"SwordTrail");

	m_vecAnimation.push_back(GetOwner()->Animator3D());

	const vector<CGameObject*> vecChilds = GetOwner()->GetChilds();
	for (UINT i = 0; i < vecChilds.size(); ++i)
		m_vecAnimation.push_back(vecChilds[i]->Animator3D());

	m_pTriangleMesh = CResMgr::GetInst()->FindRes<CMesh>(L"AreneStage_Navi_Mesh");
	if (m_pTriangleMesh->IsNavValid(vPos))
		Transform()->SetRelativePos(vPos);

	Transform()->SetRelativeRotationX(0.f);
	Transform()->SetRelativeRotationY(XM_PI);

	Set_Animation_Key(L"idle");
}

void CPlayerTestScript::tick()
{
	Attack();

	Dir();

	Jump();

	Move();

	Etc();

	if (m_bAirDash || m_bAttack || m_bJump || IsRoll() || m_bRollAttack || IsDash())
		ClearAnimation();

	TrailProcedure();
}

void CPlayerTestScript::finaltick()
{
	m_pUpperSwordObj->Transform()->SetRelativePos(TrailComponent()->GetUpperSwordPos());
	m_pBottomSwordObj->Transform()->SetRelativePos(TrailComponent()->GetBottomSwordPos());

	Vec3 vPos = Transform()->GetRelativePos();
	//플레이어 위치 Nav 보정
	if (vPos.y < 0)
	{
		//수정 필요
		if (m_pTriangleMesh->IsNavValid(vPos))
			Transform()->SetRelativePos(vPos);
	}

	if(PhysXComponent())
	if (PhysXComponent()->IsRelease())
	{
		m_bJump = false;
		Set_Animation_Key(L"idle");
		Set_Animation_Time(1.f);
	}

	CGameObjectEx* pObject = CEditor::GetInst()->FindByName(L"FogObject");
	vPos = Transform()->GetRelativePos();
	pObject->MeshRender()->GetCurMaterial(0)->SetScalarParam(VEC4_1, &vPos);
}

void CPlayerTestScript::BeginOverlap(CCollider* _pOther)
{
}

void CPlayerTestScript::Overlap(CCollider* _pOther)
{

}

void CPlayerTestScript::EndOverlap(CCollider* _pOther)
{
}

void CPlayerTestScript::SaveToFile(FILE* _File)
{
	CScript::SaveToFile(_File);
	fwrite(&m_fSpeed, sizeof(float), 1, _File);
	fwrite(&m_fJumpHeight, sizeof(float), 1, _File);

	SaveResourceRef(m_Prefab, _File);
}

void CPlayerTestScript::LoadFromFile(FILE* _File)
{
	CScript::LoadFromFile(_File);
	fread(&m_fSpeed, sizeof(float), 1, _File);
	fread(&m_fJumpHeight, sizeof(float), 1, _File);

	LoadResourceRef(m_Prefab, _File);
}

void CPlayerTestScript::Set_Animation_Key(const wstring& _strKey)
{
	for (UINT i = 0; i < m_vecAnimation.size(); ++i)
		m_vecAnimation[i]->SetCurFrameKey(_strKey);
}

void CPlayerTestScript::Set_Animation_Time(float _fTime)
{
	for (UINT i = 0; i < m_vecAnimation.size(); ++i)
		m_vecAnimation[i]->SetTimeScale(_fTime);
}
bool CPlayerTestScript::ClearAnimation()
{
	if (GetOwner()->Animator3D()->IsEnd())
	{
		if (m_bAirDash)
		{
			if (PhysXComponent()->GetRun())
				PhysXComponent()->SetType(PhysType::PISTOL);
		}

		m_bAirDash = false;
		m_bAttack = false;
		m_bJump = false;
		m_bRollAttack = false;
		Set_Animation_Key(L"idle");

		ClearRoll();
		ClearDash();

		return true;
	}
	
	if (L"idle" == GetOwner()->Animator3D()->GetCurFrameKey())
	{
		m_pSworTrailObj->TrailComponent()->ClearSwordPos();

		if (m_bAirDash)
		{
			if (PhysXComponent()->GetRun())
				PhysXComponent()->SetType(PhysType::PISTOL);
		}

		m_bAirDash = false;
		m_bAttack = false;
		m_bJump = false;
		m_bRollAttack = false;

		ClearRoll();
		ClearDash();

		return true;
	}

	return false;
}

void CPlayerTestScript::Move()
{
	if (!m_bAirDash && !m_bAttack && !m_bJump && !IsRoll() && !m_bRollAttack && !IsDash())
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
			m_eDir = CAM_DIR::FRONT;

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
			m_eDir = CAM_DIR::BACK;

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

		if (ClearAnimation())
		{
			iMoveCount = 0;
		}
	}
	
}

void CPlayerTestScript::PlayerTrail()
{
	static wstring strName = L"PayerTrail";
	static UINT iCount = 0;

	if (0xffff <= iCount)
		iCount = 0;

	CAnimator3D* pCompoent = GetOwner()->Animator3D();

	Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"PlayerTrailPrefab");
	assert(pPrefab.Get());

	CGameObject* pObj = pPrefab->Instantiate();
	CGameObjectEx* pObject = new CGameObjectEx(*pObj);
	pObject->AddComponent(pCompoent->Clone());
	pObject->AddComponent(new CTrailComponent);
	pObject->DeleteChildsAll();
	pObject->Transform()->SetRelativePos(GetOwner()->Transform()->GetRelativePos());
	pObject->Transform()->SetRelativeRotation(GetOwner()->Transform()->GetRelativeRotation());
	pObject->GetRenderComponent()->SetShadowType(ShadowType::NONE);
	pObject->Animator3D()->SetCurFrameKey(pCompoent->GetCurFrameKey());
	pObject->Animator3D()->SetCurFrameIdx(pCompoent->GetCurFrameIdx());
	pObject->Animator3D()->SetCurEndIdx(pCompoent->GetCurFrameIdx());
	pObject->Animator3D()->SetTrail(true);

	//assert(GetOwner()->TrailComponent());

	//TrailComponent()->SetData(1.f);
	m_vecTrailTime.push_back(1.f);
	delete pObj;

	pObject->SetName(strName + std::to_wstring(iCount++));

	m_vecObjTrail.push_back(pObject);
	CEditor::GetInst()->Add_Editobject(EDIT_MODE::MAPTOOL, pObject);
}

void CPlayerTestScript::Etc()
{
	static float fDashDelta = DT;

	static float fAirDashDelta = DT;
	
	static float fDashInterval = 1.f;

	static float fAirDashInterval = 2.f;

	static float fDir = 1.f;
	static float fDirOffset = -1.f;

	if (!m_bAttack && !m_bAirDash && !IsRoll() && !m_bRollAttack && !IsDash()  && KEY_TAP(KEY::LSHIFT))
	{
		if (m_bJump)
		{
			m_bAirDash = true;
			fDashDelta = 0.f;
			fAirDashInterval = 2.f;

			if (PhysXComponent()->GetRun())
				PhysXComponent()->SetType(PhysType::PAUSE);

			float dt = 0.5f;
			Animator3D()->SetTimeScale(dt);
			Set_Animation_Key(L"dash-air2");
		}
	}

	if (!m_bJump && !m_bAttack && !m_bAirDash && !IsRoll() && !m_bRollAttack && !IsDash() && KEY_PRESSED(KEY::LSHIFT))
	{
		if (KEY_PRESSED(KEY::W))
		{
			if (KEY_PRESSED(KEY::LBTN))
			{
				m_bRollAttack = true;
				float dt = 0.5f;
				Animator3D()->SetTimeScale(dt);

				Set_Animation_Key(L"roll-attack");
			}
		}
	}

	if (!m_bJump && !m_bAttack && !m_bAirDash && !IsRoll() && !m_bRollAttack && !IsDash() && KEY_PRESSED(KEY::LSHIFT))
	{
		if (KEY_PRESSED(KEY::W))
		{
			m_bRoll[0] = true;
			float dt = 0.5f;
			Animator3D()->SetTimeScale(dt);

			Set_Animation_Key(L"roll");
		}
		else if (KEY_PRESSED(KEY::S))
		{
			m_bRoll[1] = true;
			float dt = 0.5f;
			Animator3D()->SetTimeScale(dt);

			Set_Animation_Key(L"roll");
		}
		else if (KEY_PRESSED(KEY::A))
		{
			fDir = 1.f;
			fDashInterval = 1.f;
			fDashDelta = 0.f;
			m_bDash[2] = true;
			float dt = 1.5f;
			Animator3D()->SetTimeScale(dt);

			Set_Animation_Key(L"dash-left");
		}
		else if (KEY_PRESSED(KEY::D))
		{
			fDir = 1.f;
			fDashInterval = 1.f;
			fDashDelta = 0.f;
			m_bDash[3] = true;
			float dt = 1.5f;
			Animator3D()->SetTimeScale(dt);

			Set_Animation_Key(L"dash-right");
		}
	}

	if (IsRoll())
	{
		UINT idx = GetRollDir();
		Vec3 vPos{};
		Vec3 vDir{};

		if (idx == 0)
		{
			vPos = Transform()->GetRelativePos();
			vDir = Transform()->GetRelativeDir(DIR::FRONT);
			vPos -= vDir * m_fSpeed * 0.005f;
			Transform()->SetRelativePos(vPos);
		}
		else if (idx == 1)
		{
			vPos = Transform()->GetRelativePos();
			vDir = Transform()->GetRelativeDir(DIR::FRONT);
			vPos += vDir * m_fSpeed * 0.005f;
			Transform()->SetRelativePos(vPos);
		}
	}

	if (m_bAirDash)
	{
		fAirDashDelta += DT * 12.f;
		Vec3 vPos = Transform()->GetRelativePos();
		Vec3 vFront = Transform()->GetRelativeDir(DIR::FRONT);
		vPos -= vFront * m_fSpeed * 0.01f;
		Transform()->SetRelativePos(vPos);

		if (fAirDashInterval < fAirDashDelta)
		{
			fAirDashDelta -= fAirDashInterval;

			if (1.f < fAirDashInterval)
				fAirDashInterval -= 0.1f;
			else if (1.f > fAirDashInterval)
				fAirDashInterval -= 0.08f;

			PlayerTrail();
		}
	}

	if (IsDash())
	{
		UINT i = GetDashDir();

		if (i == 1)
		{
			Vec3 vPos = Transform()->GetRelativePos();
			Vec3 vFront = Transform()->GetRelativeDir(DIR::FRONT);
			Vec3 vRight = Transform()->GetRelativeDir(DIR::RIGHT);

			vPos += vFront * m_fSpeed * 0.03f;
			vPos += vRight * m_fSpeed * 0.01f * fDir;

			Transform()->SetRelativePos(vPos);
		}
		else if (i == 2)
		{
			Vec3 vPos = Transform()->GetRelativePos();
			Vec3 vFront = Transform()->GetRelativeDir(DIR::FRONT);
			Vec3 vRight = Transform()->GetRelativeDir(DIR::RIGHT);

			if (m_eDir == CAM_DIR::FRONT)
				vPos -= vFront * m_fSpeed * 0.03f * (1.f - fDir);
			else if (m_eDir == CAM_DIR::BACK)
				vPos += vFront * m_fSpeed * 0.03f * (1.f - fDir);

			vPos += vRight * m_fSpeed * 0.03f * fDir;
			Transform()->SetRelativePos(vPos);

		}
		else if (i == 3)
		{
			Vec3 vPos = Transform()->GetRelativePos();
			Vec3 vFront = Transform()->GetRelativeDir(DIR::FRONT);
			Vec3 vRight = Transform()->GetRelativeDir(DIR::RIGHT);

			if (m_eDir == CAM_DIR::FRONT)
				vPos -= vFront * m_fSpeed * 0.03f * (1.f - fDir);
			else if (m_eDir == CAM_DIR::BACK)
				vPos += vFront * m_fSpeed * 0.03f * (1.f - fDir);

			vPos -= vRight * m_fSpeed * 0.03f * fDir;
			Transform()->SetRelativePos(vPos);
		}
		
		fDir = fDir + 0.025f * fDirOffset * 0.75f;

		if (fDir <= 0)
			fDirOffset = 1.f;
		if(fDir >= 1.f)
			fDirOffset = -1.f;

		fDashDelta += DT * 8.f;

		if (fDashInterval < fDashDelta)
		{
			fDashDelta -= fDashInterval;

			if (0.5f < fDashInterval)
				fDashInterval -= 0.2f;
			else if (0.5f > fDashInterval)
				fDashInterval -= 0.08f;

			PlayerTrail();
		}
	}
}

void CPlayerTestScript::Attack()
{
	static float fAttackDelta = DT;
	static float fAttackInterval = 0.08f;

	//ClearAnimation에 걸린 경우 m_bAttack true 이므로 false 초기화
//!m_bAttack Move에서 애니메이션 방복하고, 이전 프레임에 공격이 끝났었음
	if (!(m_bAirDash) && (!m_bJump) && (!IsRoll()) && (!m_bRollAttack) && (ClearAnimation()))
	{
		if (KEY_TAP(KEY::NUM_9))
		{
			m_bAttack = true;
			fAttackDelta = 0.f;

			if (0 == m_iLButtonCount)
			{
				m_bSwordTrail = true;
				m_iLButtonCount = 1;
				Set_Animation_Key(L"ciritic-fight-1");
				Set_Animation_Time(m_fAttackDelay);
				m_pSworTrailObj->TrailComponent()->ClearSwordPos();

			}
			else if (1 == m_iLButtonCount)
			{
				m_bSwordTrail = true;
				m_iLButtonCount = 2;
				Set_Animation_Key(L"ciritic-fight-2");
				Set_Animation_Time(m_fAttackDelay);
				m_pSworTrailObj->TrailComponent()->ClearSwordPos();
			}
			else if (2 == m_iLButtonCount)
			{
				m_bSwordTrail = true;
				m_iLButtonCount = 0;
				Set_Animation_Key(L"ciritic-fight-3");
				Set_Animation_Time(m_fAttackDelay);
				m_pSworTrailObj->TrailComponent()->ClearSwordPos();
			}
		}
	}

	if (m_bAttack)
	{
		fAttackDelta += DT * 10.f;

		if (fAttackInterval < fAttackDelta)
		{
			fAttackDelta -= 0.08f;
		}
	}
}

void CPlayerTestScript::TrailProcedure()
{
	static UINT iFrameCount = 0;
	static UINT iUpdate = 0;
	static float fDeltaTime = 0.f;

	if (m_bSwordTrail)
	{
		fDeltaTime += DT;

		if (iFrameCount != Animator3D()->GetCurFrameIdx())
		{
			iFrameCount = Animator3D()->GetCurFrameIdx();
			Vec3 vUpperPos = TrailComponent()->GetUpperSwordPos();
			Vec3 vBottomPos = TrailComponent()->GetBottomSwordPos();

			m_pSworTrailObj->TrailComponent()->PushSwordPos(vUpperPos);
			m_pSworTrailObj->TrailComponent()->PushSwordPos(vBottomPos);
			iUpdate = 0;

		}
		else if (iFrameCount == Animator3D()->GetCurFrameIdx() && iUpdate < 4)
		{
			if (0.00005 < fDeltaTime)
			{
				Vec3 vUpperPos = TrailComponent()->GetUpperSwordPos();
				Vec3 vBottomPos = TrailComponent()->GetBottomSwordPos();

				m_pSworTrailObj->TrailComponent()->PushSwordPos(vUpperPos);
				m_pSworTrailObj->TrailComponent()->PushSwordPos(vBottomPos);
				++iUpdate;
				fDeltaTime = 0.f;
			}
		}
		
		if (iFrameCount == Animator3D()->GetCurFrame().iEnd)
		{
			fDeltaTime = 0.f;

			m_bSwordTrail = false;
			m_pSworTrailObj->TrailComponent()->ClearSwordPos();
		}
		
	}


	//Trail Procudure
	{
		auto objIter = m_vecObjTrail.begin();

		for (auto iter = m_vecTrailTime.begin(); iter != m_vecTrailTime.end() && objIter != m_vecObjTrail.end();)
		{
			if ((*iter) <= 0)
			{
				CEditor::GetInst()->DeleteByName(EDIT_MODE::MAPTOOL, (*objIter));
				objIter = m_vecObjTrail.erase(objIter);
				iter = m_vecTrailTime.erase(iter);
			}
			else
			{
				(*iter) -= DT;
				++iter;
				(*objIter)->TrailComponent()->SetVectorTrailTime(m_vecTrailTime);
				++objIter;
			}
		}
	}
}

void CPlayerTestScript::Jump()
{
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
}

void CPlayerTestScript::Dir()
{
	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();

		Vec3 vRot = Transform()->GetRelativeRotation();

		//vRot.x += -vMouseDir.y * DT * XM_PI;
		vRot.y += vMouseDir.x * DT * XM_PI;

		Transform()->SetRelativeRotation(vRot);
	}

}

bool CPlayerTestScript::IsRoll()
{
	for (UINT i = 0; i < 2; ++i)
	{
		if (m_bRoll[i])
			return true;
	}
	return false;
}

void CPlayerTestScript::ClearRoll()
{
	for (UINT i = 0; i < 2; ++i)
	{
		m_bRoll[i] = false;
	}
}

UINT CPlayerTestScript::GetRollDir()
{
	for (UINT i = 0; i < 2; ++i)
	{
		if (m_bRoll[i])
			return i;
	}
	return -1;
}

bool CPlayerTestScript::IsDash()
{
	for (UINT i = 0; i < 4; ++i)
	{
		if (m_bDash[i])
			return true;
	}
	return false;
}

void CPlayerTestScript::ClearDash()
{
	for (UINT i = 0; i < 4; ++i)
	{
		m_bDash[i] = false;
	}
}

UINT CPlayerTestScript::GetDashDir()
{
	for (UINT i = 0; i < 4; ++i)
	{
		if (m_bDash[i])
			return i;
	}
	return -1;
}