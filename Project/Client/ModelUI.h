#pragma once
#include "UI.h"

class TreeUI;
class TreeNode;
class CGameObjectEx;

class ModelUI :
	public UI
{
private:
	TreeUI*			m_Tree;
	string			m_strName;
	CGameObjectEx*	m_pGameObject;
private:
	virtual void update() override;
	virtual void render_update() override;

public:
	void ResetLevel();

private:
	void SetObjectToInspector(DWORD_PTR _res);
	void AddGameObjectToTree(TreeNode* _ParentNode, CGameObjectEx* _Object);

	void AddChildObject(DWORD_PTR _ChildObject, DWORD_PTR _ParentObject);

public:
	ModelUI();
	~ModelUI();
};