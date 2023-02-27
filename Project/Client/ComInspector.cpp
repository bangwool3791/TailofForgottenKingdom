#include "pch.h"
#include "ComInspector.h"

#include <Engine/CRes.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CGameObject.h>

#include "TransformUI.h"
#include "MeshRenderUI.h"
#include "Collider2DUI.h"
#include "ShadowUI.h"
#include "Light2DUI.h"
#include "EditAnimationUI.h"
/*
* Resource
*/
#include "ResUI.h"

ComInspector::ComInspector()
	: UI("ComInspector")
	, m_TargetCom(nullptr)
	, m_arrComUI{}
{
	/*
	* 자동 조절
	*/
}

ComInspector::~ComInspector()
{
}

void ComInspector::update()
{
	//SetTargetCom(m_TargetCom);
	UI::update();
}

void ComInspector::render_update()
{

}

void ComInspector::SetTargetCom(CComponent* _Target)
{
}


