#include "pch.h"
#include "CEngineBrushScript.h"

#include <Engine\CCamera.h>
#include <Engine\CScript.h>

#include <Engine\CLandScape.h>
#include <Engine\CLevelMgr.h>

CEngineBrushScript::CEngineBrushScript()
	: CScript(ENGINEBRUSHSCRIPT)
{
	SetName(L"CEngineBrushScript");


}

CEngineBrushScript::~CEngineBrushScript()
{
}


void CEngineBrushScript::begin()
{

}

void CEngineBrushScript::tick()
{

}

void CEngineBrushScript::finaltick()
{
	CGameObject* pObj = CLevelMgr::GetInst()->FindObjectByName(L"LandScape");
	
	const tRaycastOut& tRay = pObj->LandScape()->GetRay();
	
	if(tRay.bSuccess)
		this->GetOwner()->Transform()->SetRelativePos(tRay.vPos * 100.f);
}

void CEngineBrushScript::BeginOverlap(CCollider* _pOther)
{
}

void CEngineBrushScript::Overlap(CCollider* _pOther)
{
}

void CEngineBrushScript::EndOverlap(CCollider* _pOther)
{
}

void CEngineBrushScript::SaveToFile(FILE* _File)
{
	CScript::SaveToFile(_File);
}

void CEngineBrushScript::LoadFromFile(FILE* _File)
{
	CScript::LoadFromFile(_File);

}
