#include "pch.h"
#include "TreeUI.h"
#include <Engine\CKeyMgr.h>
#include <Engine\func.h>
#include <Engine\CPrefab.h>
#include <Engine\CComponent.h>
#include <Engine\CMeshRender.h>
#include <Engine\CAnimator3D.h>
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
	//���� ǥ�� ����ΰ�
	if (m_bFrame)
		iFlag |= ImGuiTreeNodeFlags_Framed;
	//���� �Ǿ��°�
	if (m_bSelected)
		iFlag |= ImGuiTreeNodeFlags_Selected;
	//Leaf ����ΰ�
	if (m_vecChildNode.empty())
		iFlag |= ImGuiTreeNodeFlags_Leaf;


	static string strName{};
	strName = m_strName;
	//���� ǥ�� ����̸鼭 Leaf����̸�
	//��ĭ Shift
	if (m_bFrame && m_vecChildNode.empty())
		strName = "\t" + strName;

	// �ڿ� �ĺ���ȣ �ٿ��� �̸�(Ű) �� ���� �ʵ��� ��
	char szTag[50] = "";
	sprintf_s(szTag, 50, "##%d", m_iIdx);
	strName += szTag;

	//Ʈ�� UI Render
	if (ImGui::TreeNodeEx(strName.c_str(), iFlag))
	{
		// �巡�� üũ
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
		{
			/*
			* Ʈ�� �巡�� ���� ��� -> ������
			* m_DragDropFunc �Լ� ������ ���, �ڽ� ��� �Է� �Ķ���� ����
			*/
			m_TreeUI->SetBeginDragNode(this);
			//Tree�� Tree Node ������ ����
			ImGui::SetDragDropPayload(m_TreeUI->GetName().c_str(), (void*)this, sizeof(TreeNode));
			//�巡�� �� ������ �̸� ���
			std::string delimiter = "##";
			std::string token = strName.substr(0, strName.find(delimiter)); 
			ImGui::Text(token.c_str());
			//Prefab�� �� Ŭ���̾�Ʈ ���� �ȿ� ������ Level ����

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
		// Ŭ�� üũ
		else if (!m_bFrame && ImGui::IsItemHovered(0) && ImGui::IsMouseClicked(0))
		{
			/*
			* ���� ���� ���
			* ��� �ȿ� �ִ� Resource �����͸� �ν����ͷ� �����Ѵ�.
			*/
			m_TreeUI->SetLBtnSelectedNode(this);
		}
		else if (!m_bFrame && ImGui::IsItemHovered(0) && ImGui::IsMouseClicked(1))
		{
			m_TreeUI->SetRBtnSelectedNode(this);
		}
		// ��� üũ(��� ���)
		if (ImGui::BeginDragDropTarget())
		{
			/*
			* m_DragDropFunc �Լ� ������ ���, �θ� ��� �Է� �Ķ���� ����
			*/
			m_TreeUI->SetDropTargetNode(this);

			ImGui::EndDragDropTarget();
		}
		/*
		* TreeNode ���� �ڽ� ��� render
		*/
		for (size_t i = 0; i < m_vecChildNode.size(); ++i)
		{
			m_vecChildNode[i]->render_update();
		}

		ImGui::TreePop();
	}

	/*
	* ��� �巡�װ� �߻��߰� ���콺 �̺�Ʈ�� �������̸�
	* ī�޶� ��ġ�� ���� �ȿ� ���콺 ������Ʈ�� ������
	* ������Ʈ ���� �̺�Ʈ
	* ī�޶� ��ġ�� ���� �ȿ� ���콺 ������Ʈ�� ������
	* ������Ʈ ���� �̺�Ʈ
	*/


}

void TreeNode::DeleteChild(TreeNode* _ChildeNode)
{
	for (auto iter{ m_vecChildNode.begin() }; iter != m_vecChildNode.end(); ++iter)
	{
		if (!strcmp((*iter)->GetName().c_str(), _ChildeNode->GetName().c_str()))
		{
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
		//�̹� ��Ʈ�� ���� �� ��尡 �ִٸ� assert
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
		//�̹� ��Ʈ�� ���� �� ��尡 �ִٸ� assert
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
	* Content UI, Outliner UI -> Tree UI, has a ���� ���
	*						  -> ���� m_SelectInst, m_Select_LBtn_Func �ʱ�ȭ
	*			 
	* Content, Outliner UI-> SetObjectToInspector �Լ� ȣ��
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
* Node, has a ���� TreeNode->SetDropTargetNode
*/
#include <Engine\CGameObject.h>

void TreeUI::SetDropTargetNode(TreeNode* _DropTargetNode)
{
	m_DropTargetNode = _DropTargetNode;

	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GetName().c_str()))
	{
		if (m_DragDropInst && m_DragDropFunc)
		{
			CGameObjectEx* pObj = (CGameObjectEx*)m_BeginDragNode->GetData();
			
			(m_DragDropInst->*m_DragDropFunc)((DWORD_PTR)m_BeginDragNode, (DWORD_PTR)m_DropTargetNode);
			DeleteNode(m_BeginDragNode);

			EDIT_MODE eMode = CEditor::GetInst()->GetEditMode();

			CEditor::GetInst()->PopByName(eMode, pObj->GetName());

		}
	}
	else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##ModelComTree"))
	{
		/*
		* Outliner UI �ܰ�, GameObject Resource �߰�.
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
	else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("##ContentTree"))
	{
		if ("##OutlinerTree" == m_DropTargetNode->GetTreeName())
		{
			TreeNode* pNode = (TreeNode*)payload->Data;
			CRes* pRes = (CRes*)pNode->GetData();

			RES_TYPE eType = pRes->GetResType();

			switch (eType)
			{
			case RES_TYPE::PREFAB:
				break;
			case RES_TYPE::MESHDATA:
			{
				Ptr<CMeshData> pMeshData = (CMeshData*)pRes;
				pMeshData->GetMaterials();
				CGameObject* pGameObject = (CGameObject*)m_DropTargetNode->GetData();

				if (nullptr != pGameObject)
				{
					if (nullptr == pGameObject->MeshRender())
						pGameObject->AddComponent(new CMeshRender);

					Ptr<CMesh> pMesh = pMeshData->GetMesh();
					pGameObject->MeshRender()->ClearMaterials();
					pGameObject->MeshRender()->SetMesh(pMesh);
					const vector<Ptr<CMaterial>>& vec_mtrls = pMeshData->GetMaterials();

					for (size_t i = 0; i < vec_mtrls.size(); ++i)
						pGameObject->MeshRender()->SetSharedMaterial(vec_mtrls[i], i);

					if (0 != pMesh->GetBones()->size() && 0 != pMesh->GetAnimClip()->size())
					{
						if (nullptr == pGameObject->Animator3D())
							pGameObject->AddComponent(new CAnimator3D);

						pGameObject->Animator3D()->begin();
						pGameObject->Animator3D()->SetBones(pMesh->GetBones());
						pGameObject->Animator3D()->SetAnimClip(pMesh->GetAnimClip());
						pGameObject->Animator3D()->SetMeshData(pMeshData);
					}
					else
					{
						if (nullptr != pGameObject->Animator3D())
						{
							pGameObject->DestroyComponent(COMPONENT_TYPE::ANIMATOR3D);
						}
					}
				}
			}
				break;
			case RES_TYPE::COMPUTE_SHADER:
				break;
			case RES_TYPE::MATERIAL:
				break;
			case RES_TYPE::MESH:
				break;
			case RES_TYPE::TEXTURE:
				break;
			case RES_TYPE::SOUND:
				break;
			case RES_TYPE::GRAPHICS_SHADER:
				break;
			case RES_TYPE::END:
				break;
			default:
				break;
			}
		}
	}
}

void TreeUI::SetSelectNode(const string& _name)
{
	TreeNode* pNode = GetNode(_name);

	assert(pNode);

	SetLBtnSelectedNode(pNode);
}

TreeNode* TreeUI::GetNode(const string& _name)
{
	const vector<TreeNode*> vecNode = m_RootNode->GetChild();

	TreeNode* pNode = nullptr;

	for (UINT i = 0; i < vecNode.size(); ++i)
	{
		pNode = GetNode(vecNode[i], _name);

		if (nullptr != pNode)
			return pNode;
	}

	return nullptr;
}

TreeNode* TreeUI::GetNode(TreeNode* _pNode, const string& _name)
{
	if (!strcmp(_pNode->GetName().c_str(), _name.c_str()))
		return _pNode;

	const vector<TreeNode*> vecNode = _pNode->GetChild();

	TreeNode* pNode = nullptr;

	for (UINT i = 0; i < vecNode.size(); ++i)
	{
		pNode = GetNode(vecNode[i], _name);

		if (nullptr != pNode)
			return pNode;
	}

	return nullptr;
}