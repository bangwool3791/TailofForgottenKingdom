#include "pch.h"
#include "CCameraScript.h"


CCameraScript::CCameraScript()
	:CScript(-1)
	, m_fSpeed(200.f)
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
	Move();
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
		Vec3 vFront = Transform()->GetRelativeDir(DIR::FRONT);
		Vec3 vRight = Transform()->GetRelativeDir(DIR::RIGHT);

		if (KEY_PRESSED(KEY::W))
			vPos += DT * vFront * fSpeed;
		if (KEY_PRESSED(KEY::S))
			vPos += DT * vFront * -fSpeed;
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

		if (KEY_PRESSED(KEY::SPACE))
		{
			Camera()->SetProjType(PROJ_TYPE::ORTHOGRAHPICS);
		}
	}

	else
	{
		Vec3 vFront = Transform()->GetRelativeDir(DIR::UP);
		Vec3 vRight = Transform()->GetRelativeDir(DIR::RIGHT);

		if (KEY_PRESSED(KEY::W))
			vPos += DT * vFront * fSpeed;
		if (KEY_PRESSED(KEY::S))
			vPos += DT * vFront * -fSpeed;
		if (KEY_PRESSED(KEY::A))
			vPos += DT * -vRight * fSpeed;
		if (KEY_PRESSED(KEY::D))
			vPos += DT * vRight * fSpeed;

		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();

		Transform()->SetRelativePos(vPos);

		if (KEY_PRESSED(KEY::NUM_1))
		{
			float fScale = Camera()->GetOrthographicScale();
			fScale += DT;
			Camera()->SetOrthographicScale(fScale);
		}
		else if (KEY_PRESSED(KEY::NUM_2))
		{
			float fScale = Camera()->GetOrthographicScale();
			fScale -= DT;
			Camera()->SetOrthographicScale(fScale);
		}

		if (KEY_PRESSED(KEY::SPACE))
		{
			Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
		}
	}
}
