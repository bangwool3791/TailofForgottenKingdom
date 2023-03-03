#include "pch.h"
#include "CFrustumScript.h"

#include <Engine\CDevice.h>
#include <Engine\CLayer.h>
#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CCollisionMgr.h>
#include <Engine\CScript.h>
#include <Engine\CCollider.h>
#include <Engine\CTransform.h>
#include <Engine\CMaterial.h>
#include <Engine\CMeshRender.h>

#include <Engine\CCamera.h>
#include <Engine\CScript.h>

CFrustumScript::CFrustumScript()
	:CScript{ FRUSTUMSCRIPT }
	, m_pFrustum{}
{
	SetName(L"CFrustumScript");

	
}

CFrustumScript::~CFrustumScript()
{
}


void CFrustumScript::begin()
{
}

void CFrustumScript::tick()
{
	if (!m_pCamera)
		m_pCamera = CLevelMgr::GetInst()->GetCurLevel()->GetLayer(0)->FindParent(L"MainCamera")->Camera();
	if (!m_pFrustum)
	{
		m_pFrustum = m_pCamera->GetFrustum();

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


	//GetOwner()->Transform()->SetRelativePos(m_pCamera->Transform()->GetRelativePos());
	//GetOwner()->Transform()->SetRelativeRotation(m_pCamera->Transform()->GetRelativeRotation());
	//GetOwner()->Transform()->SetRelativeScale(m_pCamera->Transform()->GetRelativeScale());
}

void CFrustumScript::BeginOverlap(CCollider* _pOther)
{
}

void CFrustumScript::Overlap(CCollider* _pOther)
{
}

void CFrustumScript::EndOverlap(CCollider* _pOther)
{
}

void CFrustumScript::SaveToFile(FILE* _File)
{
	CScript::SaveToFile(_File);
}

void CFrustumScript::LoadFromFile(FILE* _File)
{
	CScript::LoadFromFile(_File);

}
