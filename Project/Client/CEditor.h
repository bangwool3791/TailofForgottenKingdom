#pragma once

#include "pch.h"

class CComponent;
class CGameObjectEx;
class MemPool;
class CSLight;
/*
* 카메라 마우스 항상 동작
* Editor Object 변경
*/

class CEditor
	: public CSingleton<CEditor>
{
	//render
private:
	EDIT_MODE											m_editmode;
	/*
	* 객체 늘어나면 map<vector>처리
	*/
	CGameObject*										m_pAnimationObject;
	CGameObjectEx*										m_pCameraObject;
	vector<map<const wchar_t*, CGameObjectEx*>>			m_EditorObj;
	list<tDebugShapeInfo>								m_DebugDrawList;
	array<CGameObjectEx*, (UINT)DEBUG_SHAPE::END>		m_DebugDrawObject;
	array<CComponent*, (UINT)COMPONENT_TYPE::END>		m_arrCom;
public:
	void init();
	void progress();
public :
	void SetEditMode(EDIT_MODE _editmode);
	EDIT_MODE GetEditMode() { return m_editmode; }
private:
	void tick();
	void render();
	void debug_render();
private:
	void tickObj();
	void finaltickObj();
	void picking();
private:
	void CreateDebugDrawObject();
	void DebugDraw(tDebugShapeInfo& _info);
public:
	void UpdateAnimationObject(CGameObject* _pGameObject);
	void Add_Editobject(EDIT_MODE _emode, CGameObjectEx* _pGameObject);
	void Add_Editobject(EDIT_MODE _emode, const wchar_t* _pName, CGameObjectEx* _pGameObject);
	array<CComponent*, (UINT)COMPONENT_TYPE::END>& GetArrComponents() { return m_arrCom; }
	CComponent* GetArrComponent(COMPONENT_TYPE _eType) { return m_arrCom[(UINT)_eType]; }
	CGameObjectEx* FindByName(const wstring& _strky);
	const map<const wchar_t*, CGameObjectEx*> GetEdiotrObj(EDIT_MODE _eType) { return m_EditorObj[(UINT)_eType]; }
	void PopByName(const wstring& _strky);
private:
	CEditor();
	virtual ~CEditor();
	friend class CSingleton<CEditor>;
};