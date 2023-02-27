#include "pch.h"
#include "ModelUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CEventMgr.h>

#include "TreeUI.h"
#include "InspectorUI.h"
#include "CImGuiMgr.h"

ModelUI::ModelUI()
	: UI("ModelTree")
	, m_Tree{}
	, m_strName{}
{
	m_Tree = new TreeUI("##ModelTree");
	AddChild(m_Tree);

	m_Tree->ShowSeperator(false);

	m_Tree->SetSize(ImVec2(200.f, 200.f));
	m_Tree->AddItem(nullptr, "Model", (DWORD_PTR)nullptr);
	m_Tree->SetDummyRoot(false);

	m_Tree->AddDynamic_LBtn_Selected(this, (FUNC_1)&ModelUI::SetObjectToInspector);
}

ModelUI::~ModelUI()
{
}

void ModelUI::update()
{
	UI::update();
}

void ModelUI::render_update()
{
}

void ModelUI::ResetLevel()
{
}

void ModelUI::SetObjectToInspector(DWORD_PTR _res)
{
	/*
	* 트리 노드 게임 오브 젝트 선택 시 Inspector Component 정보 표기
	*/
	TreeNode* pNode = (TreeNode*)_res;
	m_pGameObject = (CGameObjectEx*)pNode->GetData();

	if (nullptr == m_pGameObject)
		return;

	wstring str = ((CGameObject*)m_pGameObject)->GetName();
	if (str.empty())
		return;

	m_strName = string(str.begin(), str.end());
}

void ModelUI::AddGameObjectToTree(TreeNode* _ParentNode, CGameObjectEx* _Object)
{
}

void ModelUI::AddChildObject(DWORD_PTR _ChildObject, DWORD_PTR _ParentObject)
{
}
