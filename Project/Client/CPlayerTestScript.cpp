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
	, m_fSworTrailSpeed{100.f}
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
	static float fDashDelta = DT;
	static float fAttackDelta = DT;
	static float fDashInterval = 1.f;
	static float fAttackInterval = 0.08f;

	if (!m_bAttack && !m_bAirDash && KEY_TAP(KEY::LSHIFT))
	{
		m_bAirDash = true;
		fDashDelta = 0.f;
		fDashInterval = 1.f;

		if (PhysXComponent()->GetRun())
			PhysXComponent()->SetType(PhysType::PAUSE);

		float dt = 0.5f;
		Animator3D()->SetTimeScale(dt);
		Set_Animation_Key(L"dash-air2");
	}

	if (m_bAirDash)
	{
		fDashDelta += DT * 12.f;
		Vec3 vPos = Transform()->GetRelativePos();
		Vec3 vFront = Transform()->GetRelativeDir(DIR::FRONT);
		vPos -= vFront * m_fSpeed * 0.01f;
		Transform()->SetRelativePos(vPos);

		if (fDashInterval < fDashDelta)
		{
			fDashDelta -= 1.f;

			if(0 < fDashInterval)
				fDashInterval -= 0.03f;
			
			PlayerTrail();
		}

		if (IsAnimationEnd())
		{
			if (PhysXComponent()->GetRun())
				PhysXComponent()->SetType(PhysType::PISTOL);
		}
	}

	if (m_bAttack)
	{
		fAttackDelta += DT * 10.f;

		if (fAttackInterval < fAttackDelta)
		{
			fAttackDelta -= 0.08f;

			//SwordTrail();
		}

		if (IsAnimationEnd())
		{
		}
	}

	if (!m_bAirDash && !m_bAttack && !m_bJump)
		Move();

	//IsAnimationEnd에 걸린 경우 m_bAttack true 이므로 false 초기화
	//!m_bAttack Move에서 애니메이션 방복하고, 이전 프레임에 공격이 끝났었음
	if (!(m_bAirDash) && (!m_bJump) && (IsAnimationEnd()))
	{
		if (KEY_TAP(KEY::NUM_9))
		{
			m_bAttack = true;
			fAttackDelta = 0.f;

			if (0 == m_iLButtonCount)
			{
				m_iLButtonCount = 1;
				Set_Animation_Key(L"ciritic-fight-1");
				Set_Animation_Time(m_fAttackDelay);
				m_pSworTrailObj->TrailComponent()->ClearSwordPos();

			}
			else if (1 == m_iLButtonCount)
			{
				m_iLButtonCount = 2;
				Set_Animation_Key(L"ciritic-fight-2");
				Set_Animation_Time(m_fAttackDelay);
				m_pSworTrailObj->TrailComponent()->ClearSwordPos();
			}
			else if (2 == m_iLButtonCount)
			{
				m_iLButtonCount = 0;
				Set_Animation_Key(L"ciritic-fight-3");
				Set_Animation_Time(m_fAttackDelay);
				m_pSworTrailObj->TrailComponent()->ClearSwordPos();
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

	static float fSwordTrailDT = DT;

	if (m_bAttack)
	{
		fSwordTrailDT += DT;

		Vec3 vUpperPos = TrailComponent()->GetUpperSwordPos();
		Vec3 vBottomPos = TrailComponent()->GetBottomSwordPos();

		m_pUpperSwordObj->Transform()->SetRelativePos(vUpperPos);
		m_pBottomSwordObj->Transform()->SetRelativePos(vBottomPos);

		if (1.f / m_fSworTrailSpeed < fSwordTrailDT)
		{
			static INT iCount = 0;
			++iCount;
			cout << "카운트 " << iCount << endl;
			fSwordTrailDT -= 1.f / m_fSworTrailSpeed;
			m_pSworTrailObj->TrailComponent()->PushSwordPos(vUpperPos);
			m_pSworTrailObj->TrailComponent()->PushSwordPos(vBottomPos);
		}
	}
}

void CPlayerTestScript::finaltick()
{
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
bool CPlayerTestScript::IsAnimationEnd()
{
	if (GetOwner()->Animator3D()->IsEnd())
	{
		m_bAirDash = false;
		m_bAttack = false;
		m_bJump = false;
		Set_Animation_Key(L"idle");
		return true;
	}
	
	if (L"idle" == GetOwner()->Animator3D()->GetCurFrameKey())
	{
		m_pSworTrailObj->TrailComponent()->ClearSwordPos();
		m_bAirDash = false;
		m_bAttack = false;
		m_bJump = false;
		return true;
	}

	return false;
}

void CPlayerTestScript::Move()
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

	if (IsAnimationEnd())
	{
		iMoveCount = 0;
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
	pObject->GetRenderComponent()->SetDynamicShadow(false);
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

void CPlayerTestScript::SwordTrail()
{
	static wstring strName = L"SwordTrail";
	static UINT iCount = 0;

	if (0xffff <= iCount)
		iCount = 0;

	const vector<CGameObject*>& vecChilds = GetOwner()->GetChilds();

	if (vecChilds.size() <= 2 || !vecChilds[1]->Animator3D()|| !vecChilds[1]->Transform())
		assert(false);

	Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"SwordTrailPrefab");
	assert(pPrefab.Get());

	CGameObject* pObj = pPrefab->Instantiate();
	CGameObjectEx* pObject = new CGameObjectEx(*pObj);
	pObject->AddComponent(vecChilds[1]->Animator3D()->Clone());
	pObject->Transform()->SetRelativePos(Transform()->GetRelativePos());
	pObject->GetRenderComponent()->SetDynamicShadow(false);
	pObject->Animator3D()->SetCurFrameKey(vecChilds[1]->Animator3D()->GetCurFrameKey());
	pObject->Animator3D()->SetCurFrameIdx(vecChilds[1]->Animator3D()->GetCurFrameIdx());
	pObject->Animator3D()->SetCurEndIdx(vecChilds[1]->Animator3D()->GetCurFrameIdx());
	pObject->Animator3D()->SetTrail(true);

	assert(GetOwner()->TrailComponent());

	TrailComponent()->SetSwordTrailData(1.f);

	delete pObj;

	pObject->SetName(strName + std::to_wstring(iCount++));
	m_vecSwordTrail.push_back(pObject);
	CEditor::GetInst()->Add_Editobject(EDIT_MODE::MAPTOOL, pObject);
}
