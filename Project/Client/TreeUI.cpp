#include "pch.h"
#include "TreeUI.h"
#include <Engine\CKeyMgr.h>
#include <Engine\func.h>
#include <Engine\CPrefab.h>
#include <Engine\CComponent.h>
#include "CGameObjectEx.h"
#include "CEditor.h"
// ========
// TreeNode
// ========

/*
*/
TreeNode::TreeNode()
	:m_strName{}
	, m_data{}
	, m_vecChildNode{}
	, m_ParentNode{}
	, m_TreeUI{}
	, m_iIdx{}
	, m_bFrame{}
	, m_bSelected{}
	, m_bObjectSelected{}
{
	
}

TreeNode::~TreeNode()
{
	for (auto iter{ m_vecChildNode.begin() }; iter != m_vecChildNode.end(); ++iter)
	{
		if ((*iter)->m_pGameObject)
		{
			Safe_Delete((*iter)->m_pGameObject);
		}

		Safe_Delete(*iter);
	}
}

void TreeNode::render_update()
{
	int iFlag = 0;
	//제목 표시 노드인가
	if (m_bFrame)
		iFlag |= ImGuiTreeNodeFlags_Framed;
	//선택 되었는가
	if (m_bSelected)
		iFlag |= ImGuiTreeNodeFlags_Selected;
	//Leaf 노드인가
	if (m_vecChildNode.empty())
		iFlag |= ImGuiTreeNodeFlags_Leaf;


	string strName = m_strName;
	//제목 표시 노드이면서 Leaf노드이면
	//한칸 Shift
	if (m_bFrame && m_vecChildNode.empty())
		strName = "\t" + strName;

	// 뒤에 식별번호 붙여서 이름(키) 이 같지 않도록 함
	char szTag[50] = "";
	sprintf_s(szTag, 50, "##%d", m_iIdx);
	strName += szTag;

	//트리 UI Render
	if (ImGui::TreeNodeEx(strName.c_str(), iFlag))
	{
		// 드래그 체크
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			/*
			* 트리 드래그 시작 노드 -> 현재노드
			* m_DragDropFunc 함수 포인터 사용, 자식 노드 입력 파라미터 전달
			*/
			m_TreeUI->SetBeginDragNode(this);
			//Tree에 Tree Node 데이터 전달
			ImGui::SetDragDropPayload(m_TreeUI->GetName().c_str(), (void*)this, sizeof(TreeNode));
			//드래그 시 아이템 이름 출력
			std::string delimiter = "##";
			std::string token = strName.substr(0, strName.find(delimiter)); 
			ImGui::Text(token.c_str());
			//Prefab일 때 클라이언트 영역 안에 있으면 Level 생성

			if ("##ContentTree" == m_TreeUI->GetName())
			{
				CPrefab* prefab = dynamic_cast<CPrefab*>((CPrefab*)m_data);
				if(nullptr != prefab)
				{
					m_TreeUI->SetDropObject(this);
				}

			}
			ImGui::EndDragDropSource();
		}
		// 클릭 체크
		else if (!m_bFrame && ImGui::IsItemHovered(0) && ImGui::IsMouseClicked(0))
		{
			/*
			* 유저 선택 노드
			* 노드 안에 있는 Resource 데이터를 인스펙터로 전달한다.
			*/
			m_TreeUI->SetLBtnSelectedNode(this);
		}
		else if (!m_bFrame && ImGui::IsItemHovered(0) && ImGui::IsMouseClicked(1))
		{
			m_TreeUI->SetRBtnSelectedNode(this);
		}
		// 드랍 체크(드랍 대상)
		if (ImGui::BeginDragDropTarget())
		{
			/*
			* m_DragDropFunc 함수 포인터 사용, 부모 노드 입력 파라미터 전달
			*/
			m_TreeUI->SetDropTargetNode(this);

			ImGui::EndDragDropTarget();
		}
		/*
		* TreeNode 하위 자식 노드 render
		*/
		for (size_t i = 0; i < m_vecChildNode.size(); ++i)
		{
			m_vecChildNode[i]->render_update();
		}

		ImGui::TreePop();
	}

	/*
	* 노드 드래그가 발생했고 마우스 이벤트가 릴리즈이면
	* 카메라가 비치는 영역 안에 마우스 오브젝트가 있으면
	* 오브젝트 생성 이벤트
	* 카메라가 비치는 영역 안에 마우스 오브젝트가 없으면
	* 오브젝트 해지 이벤트
	*/


}

void TreeNode::DeleteChild(TreeNode* _ChildeNode)
{
	for (auto iter{ m_vecChildNode.begin() }; iter != m_vecChildNode.end(); ++iter)
	{
		if (!strcmp((*iter)->GetName().c_str(), _ChildeNode->GetName().c_str()))
		{
			delete *iter;
			*iter = nullptr;
			m_vecChildNode.erase(iter);
			break;
		}
	}
}
const string& TreeNode::GetTreeName()
{
	return m_TreeUI->GetName();
}

// ======
// TreeUI
// ======
UINT TreeUI::m_iNextNodeIdx = 0;

TreeUI::TreeUI(const string& _strName)
	: UI(_strName)
	, m_RootNode(nullptr)
	, m_bDummyRootUse(false)
	, m_SelectedNode(nullptr)
	, m_BeginDragNode(nullptr)
	, m_DropTargetNode(nullptr)
	, m_SelectInst(nullptr)
	, m_Select_LBtn_Func(nullptr)
	, m_DragDropInst(nullptr)
	, m_DragDropFunc(nullptr)

{
}

TreeUI::~TreeUI()
{
	Safe_Del_Vec(m_vecGameObjectEx);
	Clear();
}

void TreeUI::update()
{

}

void TreeUI::render_update()
{
	if (nullptr == m_RootNode)
		return;

	if (!m_bDummyRootUse)
	{
		m_RootNode->render_update();
	}
	else
	{
		const vector<TreeNode*>& vecChildNode = m_RootNode->GetChild();

		for (size_t i{}; i < vecChildNode.size(); ++i)
		{
			vecChildNode[i]->render_update();
		}
	}

	if (ImGui::IsMouseReleased(0))
	{
		m_BeginDragNode = nullptr;
		m_DropTargetNode = nullptr;
	}
}

TreeNode* TreeUI::AddItem(TreeNode* _parent, const string& _strName, DWORD_PTR _data, bool _IsFrame)
{
	TreeNode* pNode = new TreeNode;
	pNode->SetNodeName(_strName);
	pNode->SetData(_data);
	pNode->SetFrame(_IsFrame);
	pNode->m_TreeUI = this;
	pNode->m_iIdx = m_iNextNodeIdx++;

	if (nullptr == _parent)
	{
		//이미 루트로 지정 된 노드가 있다면 assert
		assert(!m_RootNode);
		m_RootNode = pNode;
	}
	else
	{
		_parent->AddChild(pNode);
	}

	return pNode;
}

TreeNode* TreeUI::AddObjectEx(TreeNode* _parent, const string& _strName, CGameObjectEx* _pObj, bool _IsFrame)
{
	TreeNode* pNode = new TreeNode;
	pNode->SetNodeName(_strName);
	pNode->SetGameObject(_pObj);
	pNode->SetFrame(_IsFrame);
	pNode->m_TreeUI = this;
	pNode->m_iIdx = m_iNextNodeIdx++;

	if (nullptr == _parent)
	{
		//이미 루트로 지정 된 노드가 있다면 assert
		assert(!m_RootNode);
		m_RootNode = pNode;
	}
	else
	{
		_parent->AddChild(pNode);
	}

	return pNode;
}

TreeNode* TreeUI::GetNode(CGameObject* _pObj)
{
	if ((CGameObject*)m_RootNode->GetData() == _pObj)
	{
		return m_RootNode;
	}

	vector<TreeNode*>::iterator iter = m_RootNode->m_vecChildNode.begin();

	for (; iter != m_RootNode->m_vecChildNode.end(); ++iter)
	{
		if ((CGameObject*)(*iter)->GetData() == _pObj)
		{
			return (*iter);
		}
	}
	return nullptr;
}

TreeNode* TreeUI::GetNode(CGameObjectEx* _pObj)
{
	if ((CGameObjectEx*)m_RootNode->GetData() == _pObj)
	{
		return m_RootNode;
	}
	
	vector<TreeNode*>::iterator iter = m_RootNode->m_vecChildNode.begin();

	for (; iter != m_RootNode->m_vecChildNode.end(); ++iter)
	{
		if ((CGameObjectEx*)(*iter)->GetData() == _pObj)
		{
			return (*iter);
		}
	}
	return nullptr;
}

void TreeUI::DeleteNode(TreeNode* _pNode)
{
	if(_pNode)
		m_RootNode->DeleteChild(_pNode);
}

void TreeUI::Clear()
{
	if (nullptr != m_RootNode)
	{
		delete m_RootNode;
		m_RootNode = nullptr;
	}
}

void TreeUI::SetLBtnSelectedNode(TreeNode* _SelectedNode)
{
	if (nullptr != m_SelectedNode)
	{
		m_SelectedNode->m_bSelected = false;
	}

	m_SelectedNode = _SelectedNode;
	m_SelectedNode->m_bSelected = true;

	/*
	* Content UI, Outliner UI -> Tree UI, has a 관계 상속
	*						  -> 생성 m_SelectInst, m_Select_LBtn_Func 초기화
	*			 
	* Content, Outliner UI-> SetObjectToInspector 함수 호출
	*/
	if (m_SelectInst && m_Select_LBtn_Func)
	{
		(m_SelectInst->*m_Select_LBtn_Func)((DWORD_PTR)m_SelectedNode);
	}
}

void TreeUI::SetRBtnSelectedNode(TreeNode* _SelectedNode)
{
	if (nullptr != m_SelectedNode)
	{
		m_SelectedNode->m_bSelected = false;
	}

	m_SelectedNode = _SelectedNode;
	m_SelectedNode->m_bSelected = true;

	if (m_SelectInst && m_Select_RBtn_Func)
	{
		(m_SelectInst->*m_Select_RBtn_Func)((DWORD_PTR)m_SelectedNode);
	}
}

void TreeUI::SetDropObject(TreeNode* _SelectedNode)
{
	if (nullptr != m_SelectedNode)
	{
		m_SelectedNode->m_bSelected = false;
	}

	m_SelectedNode = _SelectedNode;
	m_SelectedNode->m_bSelected = true;

	if (m_SelectInst && m_DragDropWorldFunc)
	{
		(m_SelectInst->*m_DragDropWorldFunc)((DWORD_PTR)m_SelectedNode);
	}
}
/*
* Tree Drop
* Node, has a 소유 TreeNode->SetDropTargetNode
*/
#include <Engine\CGameObject.h>

void TreeUI::SetDropTargetNode(TreeNode* _DropTargetNode)
{
	m_DropTargetNode = _DropTargetNode;

	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetName().c_str()))
	{
		if (m_DragDropInst && m_DragDropFunc)
		{
			(m_DragDropInst->*m_DragDropFunc)((DWORD_PTR)m_BeginDragNode, (DWORD_PTR)m_DropTargetNode);
		}
	}
	else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##ModelComTree"))
	{
		/*
		* Outliner UI 단계, GameObject Resource 추가.
		*/

		if ("##OutlinerTree" == m_DropTargetNode->GetTreeName())
		{
			CGameObject* pGameObject = (CGameObject*)m_DropTargetNode->GetData();

			if (pGameObject)
			{
				TreeNode* pNode = (TreeNode*)payload->Data;
				CComponent* pCom = (CComponent*)pNode->GetData();
				pGameObject->AddComponent(pCom->Clone());
			}
		}
	}
}