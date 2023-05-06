#include "pch.h"
#include "CCameraScript.h"
#include "CEditor.h"
#include "CGameObjectEx.h"
#include "CPlayerTestScript.h"

#include <Engine\CTransform.h>

CCameraScript::CCameraScript()
	:CScript(-1)
	, m_fSpeed(400.f)
	, m_fDistance(700.f)
	, m_eProc(CamProc::END)
{
	SetName(L"CCameraScript");
}

CCameraScript::~CCameraScript()
{
}

void CCameraScript::begin()
{

}

void CCameraScript::tick()
{
	static bool bCamPerspective = false;

	if (!bCamPerspective)
		Move();
	else
		EditMode();

	if (KEY_TAP(KEY::NUM_0))
	{
		if (bCamPerspective)
			bCamPerspective = false;
		else
			bCamPerspective = true;
	}
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

	bool static bRot = false;
	bool static bDis = false;

	if (Camera()->GetProjType() == PROJ_TYPE::PERSPECTIVE)
	{
		//Vec3 vRight = m_pPlayer->Transform()->GetRelativeDir(DIR::RIGHT);
		//Vec3 vUp = m_pPlayer->Transform()->GetRelativeDir(DIR::UP);
		Vec3 vTarget = m_pPlayer->Transform()->GetRelativePos();
		Vec3 vRot = m_pPlayer->Transform()->GetRelativeRotation();
		static CAM_DIR eDir = m_pPlayer->GetScript<CPlayerTestScript>()->GetDir();
		CAM_DIR _eDir = m_pPlayer->GetScript<CPlayerTestScript>()->GetDir();

		//Distance
		{
			if (m_eProc == CamProc::END)
			{
				if (!bDis)
				{
					if (vPos.Distance(vPos, vTarget) >= 983.f)
					{
						float fDistance = vPos.Distance(vPos, vTarget);

						fDistance = powf(fDistance, 2);
						fDistance -= powf(vTarget.y - vPos.y, 2);
						m_fDistance = sqrtf(fDistance);
					}

					if (vPos.Distance(vPos, vTarget) >= 1200.f)
					{
						bDis = true;
					}
				}
				else
				{
					//if(100.f < vPos.Distance(vPos, vTarget))
						m_fDistance -= DT * vPos.Distance(vPos, vTarget) * 0.3f;
					//else
					//	m_fDistance -= DT * 100.f;

					if (vPos.Distance(vPos, vTarget) < 983.f)
					{
						m_fDistance = 700.f;
						bDis = false;
					}
				}
			}
			
		}

		//Roatation
		{
			if (m_eProc == CamProc::END)
			{
				if (fabsf(m_vRot.y - vRot.y) > XM_PI / 6.f)
					bRot = true;

				if (bRot)
				{
					if (fabsf(m_vRot.y - vRot.y) < XM_PI)
					{
						if (m_vRot.y < vRot.y)
							m_vRot.y += XM_PI * DT * fabsf(m_vRot.y - vRot.y);
						else
							m_vRot.y -= XM_PI * DT * fabsf(m_vRot.y - vRot.y);
					}
					else
					{
						if (m_vRot.y < vRot.y)
							m_vRot.y += (XM_2PI - XM_PI * DT * fabsf(m_vRot.y - vRot.y));
						else
							m_vRot.y -= (XM_2PI - XM_PI * DT * fabsf(m_vRot.y - vRot.y));
					}

					if (fabsf(m_vRot.y - vRot.y) < XM_PI / 180.f)
						bRot = false;
				}
			}
		}

		if (m_eProc == CamProc::STEP1 && !bDis && ! bRot)
		{
			float fFOV = Camera()->GetFOV();
			fFOV -= 0.1f;
			m_fDistance -= 20.f;

			if (fFOV <= XM_2PI / 8.f)
				fFOV = XM_2PI / 8.f;

			Camera()->SetFOV(fFOV);

			if (250.f >= m_fDistance)
			{
				m_fDistance = 250.f;
				m_eProc = CamProc::STEP2;
			}

		}else if (m_eProc == CamProc::STEP2)
		{
			float fFOV = Camera()->GetFOV();
			fFOV += 0.1f;

			m_fDistance += 5.f;

			if (fFOV >= XM_2PI / 6.f)
				fFOV = XM_2PI / 6.f;
			Camera()->SetFOV(fFOV);

			if (700.f <= m_fDistance)
			{
				m_fDistance = 700.f;
				m_eProc = CamProc::END;
			}
		}


		//vRot.y -= XM_PI;
		//vTarget.x -= m_fDistance * sinf(vRot.y);
		//vTarget.y = 700.f;
		//vTarget.z -= m_fDistance * cosf(vRot.y);
		static bool bInit = true;
		if (bDis || bInit || bRot || m_eProc != CamProc::END)
		{
			if (eDir == CAM_DIR::FRONT)
			{
				vTarget.x += m_fDistance * sinf(m_vRot.y);
				vTarget.y += 600.f;
				vTarget.z += m_fDistance * cosf(m_vRot.y);
				Transform()->SetRelativePos(vTarget);
			}
			else if (eDir == CAM_DIR::BACK)
			{
				vTarget.x -= m_fDistance * sinf(m_vRot.y);
				vTarget.y += 600.f;
				vTarget.z -= m_fDistance * cosf(m_vRot.y);
				Transform()->SetRelativePos(vTarget);
			}
			Transform()->SetRelativePos(vTarget);
			bInit = false;
		}

		if (eDir != _eDir)
		{
			eDir = _eDir;

			if (eDir == CAM_DIR::FRONT)
			{
				vTarget.x += 700.f * sinf(m_vRot.y);
				vTarget.y += 600.f;
				vTarget.z += 700.f * cosf(m_vRot.y);
				Transform()->SetRelativePos(vTarget);
			}
			else if (eDir == CAM_DIR::BACK)
			{
				vTarget.x -= 700.f * sinf(m_vRot.y);
				vTarget.y += 600.f;
				vTarget.z -= 700.f * cosf(m_vRot.y);
				Transform()->SetRelativePos(vTarget);
			}
		}

		if (eDir == CAM_DIR::FRONT)
		{
			Transform()->SetRelativeRotationX(XM_PI / 6.f);
			Transform()->SetRelativeRotationY(m_vRot.y + XM_PI);
			Transform()->SetRelativeRotationZ(m_vRot.z);
		}
		else if (eDir == CAM_DIR::BACK)
		{
			Transform()->SetRelativeRotationX(XM_PI / 6.f);
			Transform()->SetRelativeRotationY(m_vRot.y);
			Transform()->SetRelativeRotationZ(m_vRot.z);
		}
		//Transform()->SetRelativeRotation(m_vRot.x + XM_PI / 6.f, m_vRot.y, m_vRot.z);

		if (KEY_PRESSED(KEY::SPACE))
		{
			m_fDistance = 700.f;
			bRot = false;
			bDis = false;
			m_eProc = CamProc::STEP1;
		}
	}

}

void  CCameraScript::SetPlayer(CGameObjectEx* pPlayer)
{
	m_pPlayer = pPlayer;

	m_vRot = m_pPlayer->Transform()->GetRelativeRotation();
}

void  CCameraScript::EditMode()
{
	Vec3 vPos = Transform()->GetRelativePos();

	float fSpeed = m_fSpeed;

	Vec3 vFront = Transform()->GetRelativeDir(DIR::FRONT);
	Vec3 vRight = Transform()->GetRelativeDir(DIR::RIGHT);

	if (KEY_PRESSED(KEY::W))
		vPos += DT * vFront * fSpeed;
	if (KEY_PRESSED(KEY::S))
		vPos += DT * -vFront * fSpeed;
	if (KEY_PRESSED(KEY::A))
		vPos += DT * -vRight * fSpeed;
	if (KEY_PRESSED(KEY::D))
		vPos += DT * vRight * fSpeed;

	Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();


	if (KEY_PRESSED(KEY::RBTN))
	{
		Vec3 vRot = Transform()->GetRelativeRotation();

		vRot.y += vMouseDir.x * DT * XM_PI;
		vRot.x -= vMouseDir.y * DT * XM_PI;
		Transform()->SetRelativeRotation(vRot);
	}

	Transform()->SetRelativePos(vPos);
}