#include "pch.h"
#include "CCameraScript.h"
#include "CEditor.h"
#include "CGameObjectEx.h"

#include <Engine\CTransform.h>

CCameraScript::CCameraScript()
	:CScript(-1)
	, m_fSpeed(400.f)
	, m_fDistance(700.f)
	, m_eProc(CamProc::END)
{
}

CCameraScript::~CCameraScript()
{
}

void CCameraScript::begin()
{

}

void CCameraScript::tick()
{
	if (nullptr == m_pPlayer)
		m_pPlayer = CEditor::GetInst()->FindByName(L"Test0");
	else
		Move();
}

void CCameraScript::finaltick() 
{
	if (m_pPlayer->IsDead())
		m_pPlayer = nullptr;
}

void CCameraScript::Move()
{
	Vec3 vPos = Transform()->GetRelativePos();

	float fSpeed = m_fSpeed;

	if (KEY_PRESSED(KEY::LSHIFT))
	{
		fSpeed *= 5.f;
	}

	if (Camera()->GetProjType() == PROJ_TYPE::PERSPECTIVE)
	{
		//Vec3 vRight = m_pPlayer->Transform()->GetRelativeDir(DIR::RIGHT);
		//Vec3 vUp = m_pPlayer->Transform()->GetRelativeDir(DIR::UP);
		Vec3 vPos = m_pPlayer->Transform()->GetRelativePos();
		Vec3 vRot = m_pPlayer->Transform()->GetRelativeRotation();
	
		if (m_eProc == CamProc::STEP1)
		{
			float fFOV = Camera()->GetFOV();
			fFOV -= 0.1f;
			m_fDistance -= 5.f;

			if (fFOV <= XM_2PI / 8.f)
				fFOV = XM_2PI / 8.f;

			if (250.f >= m_fDistance)
			{
				m_fDistance = 250.f;
				m_eProc = CamProc::STEP2;
			}
			
			Camera()->SetFOV(fFOV);

			vRot.x += XM_PI / 5.f;

		}else if (m_eProc == CamProc::STEP2)
		{
			float fFOV = Camera()->GetFOV();
			fFOV += 0.1f;
			m_fDistance += 5.f;

			if (fFOV >= XM_2PI / 6.f)
				fFOV = XM_2PI / 6.f;
			
			if (700.f <= m_fDistance)
				m_fDistance = 700.f;

			Camera()->SetFOV(fFOV);
			vRot.x += XM_PI / 5.f;
		}
		else
		{
			vRot.x += XM_PI / 6.f;
		}
		
		vRot.y -= XM_PI;

		vPos.x -= m_fDistance * sinf(vRot.y);
		vPos.y = 700.f;
		vPos.z -= m_fDistance * cosf(vRot.y);

		Transform()->SetRelativePos(vPos);
		Transform()->SetRelativeRotation(vRot);

		if (KEY_PRESSED(KEY::SPACE))
		{
			m_eProc = CamProc::STEP1;
		}
	}

}
