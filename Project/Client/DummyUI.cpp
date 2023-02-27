#include "pch.h"
#include "DummyUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CEventMgr.h>

#include "TreeUI.h"
#include "InspectorUI.h"

#include "CEditor.h"
#include "CImGuiMgr.h"

DummyUI::DummyUI()
	: UI("DummyTree")
	, m_Tree{}
{
	m_Tree = new TreeUI("##DummyTree");
	AddChild(m_Tree);

	m_Tree->ShowSeperator(false);
	m_Tree->SetDummyRoot(true);

	//m_Tree->AddDynamic_LBtn_Selected(this, (FUNC_1)&DummyUI::SetObjectToInspector);
	//m_Tree->AddDynamic_DragDrop(this, (FUNC_2)&DummyUI::AddChildObject);
}

DummyUI::~DummyUI()
{
}

void DummyUI::begin()
{
}

void DummyUI::update()
{
	UI::update();
}

void DummyUI::render_update()
{
}

void DummyUI::ResetLevel()
{
}

void DummyUI::SetObjectToInspector(DWORD_PTR _res)
{
}

void DummyUI::AddGameObjectToTree(TreeNode* _ParentNode, CGameObject* _Object)
{
}

void DummyUI::AddChildObject(DWORD_PTR _ChildObject, DWORD_PTR _ParentObject)
{
}
