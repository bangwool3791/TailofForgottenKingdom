#pragma once
#include "UI.h"

class TreeUI;
class TreeNode;
class CGameobject;
class ComInspector;

class ModelComUI :
	public UI
{
private:
	TreeUI* m_Tree;
private:
	virtual void update() override;
	virtual void render_update() override;

public:
	void ResetLevel();

private:
	void SetObjectToInspector(DWORD_PTR _res);
	void AddGameObjectToTree(TreeNode* _ParentNode, CGameObject* _Object);

	void AddChildObject(DWORD_PTR _ChildObject, DWORD_PTR _ParentObject);

public:
	ModelComUI();
	~ModelComUI();
};