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
	{
		m_pPlayer = CEditor::GetInst()->FindByName(L"Test0");
		if (m_pPlayer)
		{
			m_vRot = m_pPlayer->Transform()->GetRelativeRotation();
		}
	}
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

	bool static bRot = false;
	bool static bDis = false;

	if (Camera()->GetProjType() == PROJ_TYPE::PERSPECTIVE)
	{
		//Vec3 vRight = m_pPlayer->Transform()->GetRelativeDir(DIR::RIGHT);
		//Vec3 vUp = m_pPlayer->Transform()->GetRelativeDir(DIR::UP);
		Vec3 vTarget = m_pPlayer->Transform()->GetRelativePos();
		Vec3 vRot = m_pPlayer->Transform()->GetRelativeRotation();

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
					if(100.f < vPos.Distance(vPos, vTarget))
						m_fDistance -= DT * vPos.Distance(vPos, vTarget);
					else
						m_fDistance -= DT * 100.f;

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
			vTarget.x -= m_fDistance * sinf(m_vRot.y - XM_PI);
			vTarget.y += 600.f;
			vTarget.z -= m_fDistance * cosf(m_vRot.y - XM_PI);
			Transform()->SetRelativePos(vTarget);
			bInit = false;
		}

		Transform()->SetRelativeRotationX(XM_PI / 6.f);
		Transform()->SetRelativeRotationY(m_vRot.y - XM_PI);
		Transform()->SetRelativeRotationZ(m_vRot.z);
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
