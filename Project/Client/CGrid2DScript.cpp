#include "pch.h"
#include "CGrid2DScript.h"

#include <Engine/CRenderMgr.h>

#include <Engine/CCamera.h>
#include <Engine/CDevice.h>
#include <Engine/CTransform.h>
#include <Engine/CMaterial.h>
#include <Engine/CMeshRender.h>

CGrid2DScript::CGrid2DScript()
	: CScript(-1)
	, m_pMainCam{}
	, m_vColor{}
	, m_fThickness{}
	, m_fGridInterval{}
{

}

CGrid2DScript::~CGrid2DScript()
{

}


void CGrid2DScript::begin()
{

}

void CGrid2DScript::tick()
{
	m_pMainCam = CRenderMgr::GetInst()->GetMainCam();

	if (!m_pMainCam)
		return; 

	if (PROJ_TYPE::PERSPECTIVE == m_pMainCam->GetProjType())
	{
		//MeshRender()->Deactivate();
	}
	else
	{
		MeshRender()->Activate();

		Vec3	vPos = m_pMainCam->Transform()->GetRelativePos();
		float	fScale = m_pMainCam->GetOrthographicScale();
		Vec2	vRenderResolution = CDevice::GetInst()->GetRenderResolution();

		MeshRender()->GetCurMaterial(0)->SetScalarParam(FLOAT_0, &fScale);
		MeshRender()->GetCurMaterial(0)->SetScalarParam(VEC4_0, &vPos);
		MeshRender()->GetCurMaterial(0)->SetScalarParam(VEC2_0, &vRenderResolution);
									 
		MeshRender()->GetCurMaterial(0)->SetScalarParam(FLOAT_1, &m_fThickness);
		MeshRender()->GetCurMaterial(0)->SetScalarParam(FLOAT_2, &m_fGridInterval);
		MeshRender()->GetCurMaterial(0)->SetScalarParam(VEC4_1, &m_vColor);
	}
}
