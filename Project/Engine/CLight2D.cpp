#include "pch.h"
#include "CLight2D.h"

#include "CRenderMgr.h"
#include "CTransform.h"

#include "CDevice.h"
#include "CCamera.h"
#include "CRenderMgr.h"

CLight2D::CLight2D()
	:CComponent(COMPONENT_TYPE::LIGHT2D)
{
	SetName(L"CLight2D");
	m_Info.iLightType = LIGHT_TYPE::POINT;
}

CLight2D::~CLight2D()
{
}

void CLight2D::finaltick()
{
	/*
	* 플레이어, 카메라의 위치를 가져와서, Register시킨다.
	*/
	if (nullptr == Transform())
		return;

	Vec3 vLightPos = Transform()->GetWorldPos();
	m_Info.vWorldPos = vLightPos;
	m_Info.vWorldDir = Transform()->GetWorldDir(DIR::RIGHT);

	/*ㄴ
	* [-800, +800] [-450, +450] 해상도에 들어온 조명만 입력 받는다.
	*/
	CCamera* pMainCam = CRenderMgr::GetInst()->GetMainCam();
	Vec3 vCamPos = pMainCam->Transform()->GetRelativePos();
	Vec2 vRenderResolution = CDevice::GetInst()->GetRenderResolution();

	if (vCamPos.x - vRenderResolution.x <= vLightPos.x
		&& vCamPos.x + vRenderResolution.x >= vLightPos.y
		&& vCamPos.y - vRenderResolution.y <= vLightPos.y
		&& vCamPos.y + vRenderResolution.y >= vLightPos.y)
	{
		CRenderMgr::GetInst()->RegisterLight2D(this);
	}
}


void CLight2D::SaveToFile(FILE* _File)
{
	COMPONENT_TYPE type = GetType();
	fwrite(&type, sizeof(UINT), 1, _File);

	fwrite(&m_Info, sizeof(tLightInfo), 1, _File);
}

void CLight2D::LoadFromFile(FILE* _File)
{
	fread(&m_Info, sizeof(tLightInfo), 1, _File);
}
