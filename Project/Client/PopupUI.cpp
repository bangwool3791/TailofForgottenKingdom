#include "pch.h"
#include "PopupUI.h"

#include <Engine\CLevelMgr.h>
#include <Engine\CGameObject.h>
/*
* Resource
*/

PopupUI::PopupUI()
	: UI("Popup")
	, m_TargetObj(nullptr)
{
	m_TreeUI = new TreeUI("PopupTree");
	m_TreeUI->AddItem(nullptr, "AAA", 0);
	AddChild(m_TreeUI);
}

PopupUI::~PopupUI()
{

}

void PopupUI::update()
{
	if (!IsValid(m_TargetObj))
	{
		SetTargetObject(nullptr);
	}

	//SetLight(nullptr);

	UI::update();
}

void PopupUI::render_update()
{

}

void PopupUI::SetTargetObject(CGameObject* _Target)
{

}

void PopupUI::SetTargetResource(CRes* _Resource)
{
	if (nullptr != m_TargetObj)
	{
		SetTargetObject(nullptr);
	}

}




