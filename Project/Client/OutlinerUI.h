#pragma once
#include "UI.h"

class TreeUI;
class TreeNode;
class CGameObject;
class PopupUI;
class OutlinerUI :
	public UI
{
private:
	CGameObjectEx* m_pTargetObj;
	string		 m_strName;
	string		 m_strComponentName;
	string	     m_strScriptName;
	TreeNode*	 m_Node;
	TreeUI*      m_Tree;
private:
	virtual void update() override;
	virtual void render_update() override;

public:
	void ResetLevel();
public:
	void SetCurrentNode(const string& _name);
private:
	void SetObject(DWORD_PTR _res);
	void SetObjectToInspector(DWORD_PTR _res);
	void AddGameObjectToTree(TreeNode* _ParentNode, CGameObject* _Object);

	void AddChildObject(DWORD_PTR _ChildObject, DWORD_PTR _ParentObject);

	bool IsOverlapObject(const string& _name);
public:
	CGameObjectEx* GetTargetObject() {
		return m_pTargetObj;
	}
public :
	OutlinerUI();
	~OutlinerUI();
};