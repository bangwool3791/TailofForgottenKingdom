#include "pch.h"
#include "CSecondCameraScript.h"


CSecondCameraScript::CSecondCameraScript()
	:CScript(SCRIPT_TYPE::SECONDCAMERASCRIPT)
	, m_fSpeed(300.f)
	, m_vPos{}
{
}

CSecondCameraScript::~CSecondCameraScript()
{
}

void CSecondCameraScript::begin()
{
	m_fScale = Camera()->GetOrthographicScale();
}

void CSecondCameraScript::tick()
{
	Move();
}

void CSecondCameraScript::Move()
{
	static float fdt = DT;
	fdt += DT;

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

		if (KEY_PRESSED(KEY::NUM_8))
			vPos += DT * vFront * fSpeed;
		if (KEY_PRESSED(KEY::NUM_5))
			vPos += DT * vFront * -fSpeed;
		if (KEY_PRESSED(KEY::NUM_4))
			vPos += DT * -vRight * fSpeed;
		if (KEY_PRESSED(KEY::NUM_6))
			vPos += DT * vRight * fSpeed;

		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();


		if (KEY_PRESSED(KEY::LBTN))
		{
			Vec3 vRot = Transform()->GetRelativeRotation();

			vRot.y += vMouseDir.x * DT * XM_PI;
			vRot.x -= vMouseDir.y * DT * XM_PI;
			Transform()->SetRelativeRotation(vRot);
		}
		Transform()->SetRelativePos(vPos);
	}
}
