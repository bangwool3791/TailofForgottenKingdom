#include "pch.h"
#include "CBrushScript.h"

#include "CEditor.h"
#include "CGameObjectEx.h"

#include <Engine\CCamera.h>
#include <Engine\CScript.h>

#include <Engine\CLandScape.h>

CBrushScript::CBrushScript()
	: CScript(-1)
{
	SetName(L"CBrushScript");


}

CBrushScript::~CBrushScript()
{
}


void CBrushScript::begin()
{

}

void CBrushScript::tick()
{
}

void CBrushScript::finaltick()
{
	CGameObjectEx* pObj = CEditor::GetInst()->FindByName(L"EditLandScape");
	
	const tRaycastOut& tRay = pObj->LandScape()->GetRay();
	
	if(tRay.bSuccess)
		this->GetOwner()->Transform()->SetRelativePos(tRay.vPos * g_LandScale);
}

void CBrushScript::BeginOverlap(CCollider* _pOther)
{
}

void CBrushScript::Overlap(CCollider* _pOther)
{
}

void CBrushScript::EndOverlap(CCollider* _pOther)
{
}

void CBrushScript::SaveToFile(FILE* _File)
{
	CScript::SaveToFile(_File);
}

void CBrushScript::LoadFromFile(FILE* _File)
{
	CScript::LoadFromFile(_File);

}
