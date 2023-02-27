#include "pch.h"
#include "CAniPoint.h"

#include "CDevice.h"
#include "CLevel.h"
#include "CLevelMgr.h"
#include "CKeyMgr.h"
#include "CTransform.h"


CAniPoint::CAniPoint()
	:bClicked{ false }
	, binit{true}
{

}

CAniPoint::~CAniPoint()
{

}


void CAniPoint::begin()
{
	m_pCamera = CLevelMgr::GetInst()->GetCurLevel()->FindParentObjectByName(L"MainCamera");
}

void CAniPoint::tick()
{
	if (!binit)
		return;

	if (m_pCamera)
	{
		m_fCameraScale = m_pCamera->Camera()->GetOrthographicScale();
		m_vCameraPos = m_pCamera->Transform()->GetRelativePos();
	}

	m_vMousePos = CKeyMgr::GetInst()->GetMousePos();
	m_vRenderResolution = CDevice::GetInst()->GetRenderResolution();

	/*
	* 마우스 처음 눌림 상태
	*/
	if (KEY_PRESSED(KEY::LBTN) && !bClicked)
	{
		/*
		* -해상도.폭, +해상도.높이
		*/
		StartPos = Vec2{ m_vMousePos.x - m_vRenderResolution.x / 2.f, -m_vMousePos.y + m_vRenderResolution.y / 2.f };
		StartPos *= m_fCameraScale;
		StartPos += m_vCameraPos;
		bClicked = true;
	}

	EndPos = Vec2{ m_vMousePos.x - m_vRenderResolution.x / 2.f , -m_vMousePos.y + m_vRenderResolution.y / 2.f };
	EndPos *= m_fCameraScale;
	EndPos += m_vCameraPos;
	Vec2 SumPos = StartPos + EndPos;

	SumPos /= 2.f;
	m_vPos = Vec3{ SumPos.x,SumPos.y, 0.f };

	Vec3 vScale = Vec3{ std::fabsf(EndPos.x - StartPos.x), std::fabsf(EndPos.y - StartPos.y), 0.f };

	if (KEY_PRESSED(KEY::LBTN) && bClicked)
	{
		Transform()->SetRelativePos(m_vPos);
		Transform()->SetRelativeScale(vScale);
	}

	if (KEY_RELEASE(KEY::LBTN) && bClicked)
	{
		bClicked = false;
		binit = false;
		Transform()->SetRelativePos(m_vPos);
		Transform()->SetRelativeScale(vScale);
	}
}

void CAniPoint::finaltick()
{
}

void CAniPoint::BeginOverlap(CCollider2D* _pOther)
{
}

void CAniPoint::Overlap(CCollider2D* _pOther)
{

}

void CAniPoint::EndOverlap(CCollider2D* _pOther)
{

}
