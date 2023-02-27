#include "pch.h"
#include "ModelComUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>

#include "CEditor.h"
#include "TreeUI.h"
#include "ComInspector.h"
#include "CImGuiMgr.h"

ModelComUI::ModelComUI()
	: UI("ModelComUI")
	, m_Tree{}
{
	m_Tree = new TreeUI("##ModelComTree");
	AddChild(m_Tree);

	m_Tree->ShowSeperator(false);
	m_Tree->SetDummyRoot(true);

	m_Tree->AddDynamic_LBtn_Selected(this, (FUNC_1)&ModelComUI::SetObjectToInspector);
	m_Tree->AddDynamic_DragDrop(this, (FUNC_2)&ModelComUI::AddChildObject);

	array<CComponent*, (UINT)COMPONENT_TYPE::END> arr = CEditor::GetInst()->GetArrComponents();

	TreeNode* pNode{};
	pNode = m_Tree->AddItem(nullptr, " ", (DWORD_PTR)nullptr);
	m_Tree->SetDummyRoot(true);

	for (UINT i{}; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		if (i == (UINT)COMPONENT_TYPE::COLLIDER3D 
			|| i == (UINT)COMPONENT_TYPE::CAMERA
			|| i == (UINT)COMPONENT_TYPE::ANIMATOR3D
			|| i == (UINT)COMPONENT_TYPE::LIGHT3D
			|| i == (UINT)COMPONENT_TYPE::TILEMAP
			|| i == (UINT)COMPONENT_TYPE::SKYBOX
			|| i == (UINT)COMPONENT_TYPE::DECAL
			|| i == (UINT)COMPONENT_TYPE::LANDSCAPE
			|| i == (UINT)COMPONENT_TYPE::SCRIPT)
			continue;

		m_Tree->AddItem(pNode, ToString((COMPONENT_TYPE)i), (DWORD_PTR)arr[i]);
	}
		

	/*
	* 컴포넌트 상태 UI
	* 타겟 세팅
	* 타겟 enum값 추출
	* 해당 UI Open
	*/
}

ModelComUI::~ModelComUI()
{
}

void ModelComUI::update()
{
	UI::update();
}

void ModelComUI::render_update()
{
}

void ModelComUI::ResetLevel()
{

}

void ModelComUI::SetObjectToInspector(DWORD_PTR _res)
{
	//ComInspector* pUI = (ComInspector*)CImGuiMgr::GetInst()->FindUI("ComInspector");
	//TreeNode* pNode = (TreeNode*)_res;
	//CComponent* pCom = (CComponent*)pNode->GetData();
	//pUI->SetTargetCom(pCom);
}

void ModelComUI::AddGameObjectToTree(TreeNode* _ParentNode, CGameObject* _Object)
{
	string strObjectName = string(_Object->GetName().begin(), _Object->GetName().end());
	TreeNode* pCurNode = m_Tree->AddItem(_ParentNode, strObjectName.c_str(), (DWORD_PTR)_Object);

	const vector<CGameObject*>& vecChild = _Object->GetChilds();

	for (size_t i{}; i < vecChild.size(); ++i)
	{
		AddGameObjectToTree(pCurNode, vecChild[i]);
	}
}

void ModelComUI::AddChildObject(DWORD_PTR _ChildObject, DWORD_PTR _ParentObject)
{
}
