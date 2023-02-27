#pragma once
#include "UI.h"

class TreeUI;
class CGameObjectEx;

class TreeNode
{
private:
	//메모리 정리를 위한 멤버 변수
	CGameObjectEx*		m_pGameObject;
	string				m_strName;
	DWORD_PTR			m_data;

	vector<TreeNode*>	m_vecChildNode;
	TreeNode*			m_ParentNode;
	TreeUI*				m_TreeUI;

	UINT				m_iIdx;

	bool				m_bFrame;
	bool				m_bSelected;
	bool				m_bObjectSelected;

public :
	DWORD_PTR GetData() { return m_data; }
	CGameObjectEx* GetGameObjectEx() { return m_pGameObject; }
	const string& GetName() { return m_strName; }
	void SetNodeName(const string& _name) { m_strName = _name; }
private:
	void render_update();
	void SetData(DWORD_PTR _data) { m_data = _data; }
	void SetGameObject(CGameObjectEx* _pObj) { m_pGameObject = _pObj; }
	void SetFrame(bool _bFrame) { m_bFrame = _bFrame; }
	void AddChild(TreeNode* _ChildNode)
	{
		_ChildNode->m_ParentNode = this;
		m_vecChildNode.push_back(_ChildNode);
	}
	void DeleteChild(TreeNode* _ChildeNode);
	const vector<TreeNode*>& GetChild() { return m_vecChildNode; }
	const string& GetTreeName();
public:
	TreeNode();
	~TreeNode();

	friend class TreeUI;

};

class TreeUI :
	public UI
{
private:
	static UINT			    m_iNextNodeIdx;
	TreeUI*					m_ChildTreeUI;
private:
	bool					m_bUpdate;
	TreeNode*				m_RootNode;
	bool					m_bDummyRootUse;	//루트 노드를 더미로 취급할 것인가

	TreeNode*				m_SelectedNode;
	TreeNode*				m_BeginDragNode;
	TreeNode*				m_DropTargetNode;

	UI*						m_SelectInst;
	FUNC_1					m_Select_LBtn_Func;
	FUNC_1					m_Select_RBtn_Func;

	FUNC_1					m_DragDropWorldFunc;

	UI*						m_DragDropInst;
	FUNC_2					m_DragDropFunc;

	vector<CGameObjectEx*>  m_vecGameObjectEx;

public:
	virtual void update() override;
	virtual void render_update() override;

public:
	TreeNode* AddItem(TreeNode* _parent, const string& _strName, DWORD_PTR _data, bool _IsFrame = false);
	TreeNode* AddObjectEx(TreeNode* _parent, const string& _strName, CGameObjectEx* _pObj, bool _IsFrame = false);
	void Clear();
	void SetDummyRoot(bool _bUse) { m_bDummyRootUse = _bUse; }
	void SetLBtnSelectedNode(TreeNode* _SelectedNode);
	void SetRBtnSelectedNode(TreeNode* _SelectedNode);
	void SetBeginDragNode(TreeNode* _beginDragNode) { m_BeginDragNode = _beginDragNode; }
	void SetDropObject(TreeNode* _DropTargetNode);
	void SetDropTargetNode(TreeNode* _DropTargetNode);
	void SetUpdate(bool _bUpdate) { m_bUpdate = _bUpdate; }
	void CreateChild(const string& _name, const vector<string>& _vec);
	TreeNode* GetSelectedNode() { return m_SelectedNode; }

	void AddDynamic_LBtn_Selected(UI* _Inst, FUNC_1 _Func)
	{
		m_SelectInst = _Inst;
		m_Select_LBtn_Func = _Func;
	}

	void AddDynamic_RBtn_Selected(UI* _Inst, FUNC_1 _Func)
	{
		m_SelectInst = _Inst;
		m_Select_RBtn_Func = _Func;
	}

	void AddDynamic_DragDrop(UI* _Inst, FUNC_2 _Func)
	{
		m_DragDropInst = _Inst;
		m_DragDropFunc = _Func;
	}

	void AddDynamic_DragDrop_World(UI* _Inst, FUNC_1 _Func)
	{
		m_DragDropInst = _Inst;
		m_DragDropWorldFunc = _Func;
	}
	TreeNode* GetNode(CGameObject* _pObj);
	TreeNode* GetNode(CGameObjectEx* _pObj);
	void DeleteNode(TreeNode* _pNode);
public:
	TreeUI(const string& _strName);
	~TreeUI();

};