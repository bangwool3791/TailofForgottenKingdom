#include "pch.h"
#include "CLevelCameraScript.h"
#include <Engine\CLevelMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CCamera.h>

CLevelCameraScript::CLevelCameraScript()
	:CScript(SCRIPT_TYPE::LEVELCAMERASCRIPT)
	, m_fSpeed(300.f)
	, m_vPos{}
{
	AddScriptParam(SCRIPT_PARAM::FLOAT, "MoveSpeed", &m_fSpeed);
	AddScriptParam(SCRIPT_PARAM::FLOAT, "FOV", &m_fFOV);
}

CLevelCameraScript::~CLevelCameraScript()
{
}

void CLevelCameraScript::begin()
{
	m_fScale = Camera()->GetOrthographicScale();
}

void CLevelCameraScript::tick()
{
	if (!m_pCamera)
	{
		m_pCamera = CLevelMgr::GetInst()->GetCurLevel()->GetLayer(0)->FindParent(L"MainCamera")->Camera();
		assert(m_pCamera);
	}
	if (!m_pFrustum)
	{
		m_pFrustum = m_pCamera->GetFrustum();
		m_fFar = m_pCamera->GetFar();
		assert(m_pCamera);
	}

	Move();
}

void CLevelCameraScript::Move()
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

		bool bCheck = false;
		if (KEY_PRESSED(KEY::W))
		{
			vPos += DT * vFront * fSpeed;
			bCheck = true;
		}
		if (KEY_PRESSED(KEY::S))
		{
			vPos += DT * vFront * -fSpeed;
			bCheck = true;
		}
		if (KEY_PRESSED(KEY::A))
		{
			vPos += DT * -vRight * fSpeed;
			bCheck = true;
		}
		if (KEY_PRESSED(KEY::D))
		{
			vPos += DT * vRight * fSpeed;
			bCheck = true;
		}

		Vec2 vMouseDir = CKeyMgr::GetInst()->GetMouseDir();
		Transform()->SetRelativePos(vPos);

		if (KEY_PRESSED(KEY::NUM_1))
		{
			Vec3 fRot = Transform()->GetRelativeRotation();

			fRot.y += DT * XM_PI * 0.2;
			Transform()->SetRelativeRotation(fRot);
			bCheck = true;
		}

		if (KEY_PRESSED(KEY::NUM_2))
		{
			Vec3 fRot = Transform()->GetRelativeRotation();

			fRot.y -= DT * XM_PI * 0.2;
			Transform()->SetRelativeRotation(fRot);
			bCheck = true;
		}

		if (KEY_PRESSED(KEY::RBTN))
		{
			Vec3 vRot = Transform()->GetRelativeRotation();

			vRot.y += vMouseDir.x * DT * XM_PI;
			vRot.x -= vMouseDir.y * DT * XM_PI;
			Transform()->SetRelativeRotation(vRot);
			bCheck = true;
		}

		if (bCheck)
		{
			m_pCamera->Transform()->finaltick();
			m_pCamera->CalcViewMat();
			m_pCamera->CalcProjMat();
			m_pFrustum->finaltick();

			int iSize = 0;
			size_t iVtxSize = 0;
			Vec3* points = m_pFrustum->GetWorldArray(iSize);
			Vtx* vtx = GetOwner()->GetRenderComponent()->GetMesh()->GetVertices(iVtxSize);
			////À­¸é
			vtx[0].vPos = points[4];
			vtx[1].vPos = points[5];
			vtx[2].vPos = points[1];
			vtx[3].vPos = points[0];
			//¾Æ·§¸é
			vtx[4].vPos = points[3];
			vtx[5].vPos = points[2];
			vtx[6].vPos = points[6];
			vtx[7].vPos = points[7];

			vtx[8].vPos = points[4];
			vtx[9].vPos = points[0];
			vtx[10].vPos = points[3];
			vtx[11].vPos = points[7];

			vtx[12].vPos = points[1];
			vtx[13].vPos = points[5];
			vtx[14].vPos = points[6];
			vtx[15].vPos = points[2];

			vtx[16].vPos = points[4];
			vtx[17].vPos = points[5];
			vtx[18].vPos = points[6];
			vtx[19].vPos = points[7];

			vtx[20].vPos = points[0];
			vtx[21].vPos = points[1];
			vtx[22].vPos = points[2];
			vtx[23].vPos = points[3];

			GetOwner()->GetRenderComponent()->GetMesh()->UpdateVertex(vtx, iVtxSize);
		}

		if (1 == g_ScrollUp || 1 == g_ScrollDown)
		{
			m_fFOV = m_pCamera->GetFOV();

			if (1 == g_ScrollUp)
				m_fFOV += 0.01;
			else if (1 == g_ScrollDown)
				m_fFOV -= 0.01;

			m_pCamera->SetFOV(m_fFOV);

			m_pCamera->CalcViewMat();
			m_pCamera->CalcProjMat();
			m_pFrustum->finaltick();

			g_ScrollDown = 0;
			g_ScrollUp = 0;

			int iSize = 0;
			size_t iVtxSize = 0;
			Vec3* points = m_pFrustum->GetWorldArray(iSize);
			Vtx* vtx = GetOwner()->GetRenderComponent()->GetMesh()->GetVertices(iVtxSize);
			////À­¸é
			vtx[0].vPos = points[4];
			vtx[1].vPos = points[5];
			vtx[2].vPos = points[1];
			vtx[3].vPos = points[0];
			//¾Æ·§¸é
			vtx[4].vPos = points[3];
			vtx[5].vPos = points[2];
			vtx[6].vPos = points[6];
			vtx[7].vPos = points[7];

			vtx[8].vPos = points[4];
			vtx[9].vPos = points[0];
			vtx[10].vPos = points[3];
			vtx[11].vPos = points[7];

			vtx[12].vPos = points[1];
			vtx[13].vPos = points[5];
			vtx[14].vPos = points[6];
			vtx[15].vPos = points[2];

			vtx[16].vPos = points[4];
			vtx[17].vPos = points[5];
			vtx[18].vPos = points[6];
			vtx[19].vPos = points[7];

			vtx[20].vPos = points[0];
			vtx[21].vPos = points[1];
			vtx[22].vPos = points[2];
			vtx[23].vPos = points[3];

			GetOwner()->GetRenderComponent()->GetMesh()->UpdateVertex(vtx, iVtxSize);
		}

		if (KEY_PRESSED(KEY::SPACE))
		{
			if (fdt > 0.5f)
			{
				Camera()->SetProjType(PROJ_TYPE::ORTHOGRAHPICS);
				Camera()->SetOrthographicScale(m_fScale);
				Transform()->SetRelativeRotation(Vec3(XM_PI * 0.25f, 0.f, 0.f));
				fdt = 0.f;
			}
		}

		
		DebugDrawFrustum(Vec4(0.2f, 0.8f, 0.2f, 1.f), Transform()->GetWorldPos(), Transform()->GetRelativeRotation(), 0.f);
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
			m_fScale = Camera()->GetOrthographicScale();
			m_fScale += DT;
			Camera()->SetOrthographicScale(m_fScale);
		}
		else if (KEY_PRESSED(KEY::NUM_2))
		{
			m_fScale = Camera()->GetOrthographicScale();
			m_fScale -= DT;
			Camera()->SetOrthographicScale(m_fScale);
		}

		if (KEY_PRESSED(KEY::SPACE))
		{
			if (fdt > 0.5f)
			{
				m_vPos = Transform()->GetRelativeDir(DIR::FRONT);
				Camera()->SetProjType(PROJ_TYPE::PERSPECTIVE);
				fdt = 0.f;
			}
		}
	}
}
