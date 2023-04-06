#include "pch.h"
#include "OutlinerUI.h"

#include <Engine\CGameObject.h>
#include <Engine\CResMgr.h>
#include <Engine\CLevel.h>
#include <Engine\CLevelMgr.h>
#include <Engine\CEventMgr.h>

#include "CEditor.h"
#include "PopupUI.h"
#include "TreeUI.h"
#include "InspectorUI.h"
#include "CImGuiMgr.h"

OutlinerUI::OutlinerUI()
	: UI("Outliner")
	, m_Tree{}
{
	m_Tree = new TreeUI("##OutlinerTree");
	AddChild(m_Tree);

	m_Tree->ShowSeperator(false);
	m_Tree->SetDummyRoot(true);

	m_Tree->AddDynamic_RBtn_Selected(this, (FUNC_1)&OutlinerUI::SetObject);
	m_Tree->AddDynamic_LBtn_Selected(this, (FUNC_1)&OutlinerUI::SetObjectToInspector);
	m_Tree->AddDynamic_DragDrop(this, (FUNC_2)&OutlinerUI::AddChildObject);

	m_strName.resize(100);
	m_strComponentName.resize(100);
	m_strScriptName.resize(100);
	// 레벨 오브젝트 갱신
	//ResetLevel();
}

OutlinerUI::~OutlinerUI()
{
}

void OutlinerUI::update()
{
	if (CEventMgr::GetInst()->IsLevelChanged())
	{
		ResetLevel();
	}
}

void OutlinerUI::render_update()
{
	if (EDIT_MODE::ANIMATOR == CEditor::GetInst()->GetEditMode())
		return;

	//m_Tree->render_update();

	static bool toggles[] = { true, false, false, false, false };
	static const char* names[] = { "Create Empty","Destroy", "Create Prefab", "Edit Name", "Delete Component", "Delete Script" ,"Close" };
	static int IobjectIndex = 0;
	static int ILayerIndex = 0;
	int selected_fish = -1;
	
	if (ImGui::IsMouseClicked(1))
	{
		ImGui::OpenPopup("MouseList");
	}

	ImGui::SameLine();

	if (ImGui::BeginPopup("MouseList"))
	{
		for (int i = 0; i < IM_ARRAYSIZE(names); i++)
		{
			if (ImGui::Selectable(names[i]))
				selected_fish = i;
			if (i > 1)
				ImGui::Separator();
		}
		ImGui::EndPopup();

		switch (selected_fish)
		{
		case 0:
		{
			tEvent evn = {};
			evn.eType = EVENT_TYPE::CREATE_OBJECT;
			CGameObject* pGameObject = new CGameObject;
			evn.wParam = (DWORD_PTR)pGameObject;
			evn.lParam = (DWORD_PTR)(ILayerIndex);

			if (m_strName.empty())
			{
				m_strName = "DummyObject";
				m_strName += std::to_string(IobjectIndex);
				++IobjectIndex;
			}
			pGameObject->SetName(StringToWString(m_strName));
			CEventMgr::GetInst()->AddEvent(evn);
		}
		break;
		case 1:
		{
			if (nullptr != m_Node)
			{
				tEvent evn = {};
				evn.eType = EVENT_TYPE::DELETE_OBJECT;
				evn.wParam = m_Node->GetData();
				CEventMgr::GetInst()->AddEvent(evn);
				m_Tree->DeleteNode(m_Node);

				//인스펙터 타겟 오브젝트 삭제 시 
				InspectorUI* Inspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
				Inspector->SetTargetObject(nullptr);
				m_Node = nullptr;
			}
		}
		break;
		case 2:
		{
			if (nullptr != m_Node)
			{
				tEvent evn = {};
				evn.eType = EVENT_TYPE::EDIT_RES;
				evn.wParam = (DWORD_PTR)RES_TYPE::PREFAB;
				evn.lParam = (DWORD_PTR)(m_Node->GetData());
				CEventMgr::GetInst()->AddEvent(evn);
				m_Node = nullptr;
			}
		}
		break;

		case 3:
		{
			if (nullptr != m_Node)
			{
				m_Node->SetNodeName(m_strName);
				wstring lstrname = wstring(m_strName.begin(), m_strName.end());
				((CGameObject*)(m_Node->GetData()))->SetName(lstrname);
				m_Node = nullptr;
			}
		}
		break;
		case 4:
		{
			if (nullptr != m_Node)
			{
				if (((CGameObject*)(m_Node->GetData()))->DeleteComponent(m_strComponentName))
				{
					m_Node = nullptr;
				}
				else
				{
					MessageBox(nullptr, L"Fail to Delete Component", L"Error", MB_OK);
				}
			}
		}
		break;
		case 5:
		{
			if (nullptr != m_Node)
			{
				if (((CGameObject*)(m_Node->GetData()))->DeleteScript(m_strScriptName))
				{
					//인스펙터 타겟 오브젝트 삭제 시 
					InspectorUI* Inspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");
					Inspector->InitializeScriptUI();
					m_Node = nullptr;
				}
				else
				{
					MessageBox(nullptr, L"Fail to Delete Script", L"Error", MB_OK);
				}
			}
		}
		break;
		default:
			break;
		}
	}



	ImGui::Text("Object Name");
	ImGui::SameLine();
	ImGui::PushItemWidth(100.f);

	if (ImGui::InputText("##ObjectName", m_strName.data(), m_strName.size()))
	{
		if ('\0' == m_strName[0])
		{
			m_strName.clear();
			m_strName.shrink_to_fit();
		}
	}

	ImGui::NewLine();

	ImGui::Text("Component Name");
	ImGui::SameLine();
	ImGui::PushItemWidth(100.f);
	ImGui::InputText("##ComponentName", m_strComponentName.data(), 100);
	ImGui::NewLine();

	ImGui::Text("Script Name");
	ImGui::SameLine();
	ImGui::PushItemWidth(100.f);
	ImGui::InputText("##ScriptName", m_strScriptName.data(), m_strScriptName.size());
}

void OutlinerUI::ResetLevel()
{
	m_Tree->Clear();
	CLevel* pCurLevel;
	TreeNode* pRootNode{};
	static string strLevelName;

	if (EDIT_MODE::OBJECT == CEditor::GetInst()->GetEditMode())
	{
		pCurLevel = CLevelMgr::GetInst()->GetCurLevel();
		strLevelName = string(pCurLevel->GetName().begin(), pCurLevel->GetName().end());

		pRootNode = m_Tree->AddItem(nullptr, strLevelName, 0, true);

		for (UINT i{}; i < MAX_LAYER; ++i)
		{
			const vector<CGameObject*>& vecObejct = pCurLevel->GetLayer(i)->GetParentObjects();

			for (size_t j{}; j < vecObejct.size(); ++j)
			{
				AddGameObjectToTree(pRootNode, vecObejct[j]);
			}
		}
	}
	else if (EDIT_MODE::MAPTOOL == CEditor::GetInst()->GetEditMode())
	{
		strLevelName = "map object";
		pRootNode = m_Tree->AddItem(nullptr, strLevelName, 0, true);

		auto map = CEditor::GetInst()->GetEdiotrObj(EDIT_MODE::MAPTOOL);
		vector<CGameObjectEx*> vecObejct;

		for (auto iter = map.begin(); iter != map.end(); ++iter)
		{
			vecObejct.push_back(iter->second);
		}

		for (size_t j{}; j < vecObejct.size(); ++j)
		{
			AddGameObjectToTree(pRootNode, (CGameObject*)vecObejct[j]);
		}
	}

}

void OutlinerUI::SetObject(DWORD_PTR _res)
{
	m_Node = (TreeNode*)_res;
	m_pTargetObj = m_Node->GetGameObjectEx();
	assert(m_pTargetObj);
}

void OutlinerUI::SetObjectToInspector(DWORD_PTR _res)
{
	// _res : 클릭한 노드
	TreeNode* pSelectedNode = (TreeNode*)_res;
	CGameObjectEx* pObject = (CGameObjectEx*)pSelectedNode->GetData();

	// InspectorUI 에 클릭된 Resouce 를 알려준다.
	InspectorUI* Inspector = (InspectorUI*)CImGuiMgr::GetInst()->FindUI("Inspector");

	Inspector->SetTargetObject(pObject);
}

void OutlinerUI::AddGameObjectToTree(TreeNode* _ParentNode, CGameObject* _Object)
{
	string strObjectName = string(_Object->GetName().begin(), _Object->GetName().end());
	TreeNode* pCurNode = m_Tree->AddItem(_ParentNode, strObjectName.c_str(), (DWORD_PTR)_Object);

	const vector<CGameObject*>& vecChild = _Object->GetChilds();

	for (size_t i{}; i < vecChild.size(); ++i)
	{
		AddGameObjectToTree(pCurNode, vecChild[i]);
	}
}

void OutlinerUI::AddChildObject(DWORD_PTR _ChildObject, DWORD_PTR _ParentObject)
{
	TreeNode* pChildNode = (TreeNode*)_ChildObject;
	TreeNode* pParentNode = (TreeNode*)_ParentObject;

	tEvent evt = {};
	evt.eType = EVENT_TYPE::ADD_CHILD;
	evt.wParam = pChildNode->GetData();
	evt.lParam = pParentNode->GetData();
	CEventMgr::GetInst()->AddEvent(evt);
}
