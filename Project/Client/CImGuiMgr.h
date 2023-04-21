#pragma once

#include "imgui.h"

class UI;
class CGameObjectEx;
class CImGuiMgr
	: public CSingleton<CImGuiMgr>
{
private:
	unordered_map<string, UI*>	m_mapUI;
	HANDLE				m_NotifyHandle;
	CGameObjectEx*		m_pSelectedObj = nullptr;
	bool				m_bMenuClick = false;
public:
	UI* FindUI(const string& _name);

public:	
	void init(HWND _hWnd);
	void progress();
	void Guizmo();
	void SetPickingObj(CGameObjectEx* pObj) { m_pSelectedObj = pObj; }
	void SetNullPickingObj() { m_pSelectedObj = nullptr; }
	void SetMenuClick(bool _bClick) { m_bMenuClick = _bClick; }
	bool GetMenuClick() { return m_bMenuClick; }
private:
	void clear();
	void CreateUI();
	void ObserveContent();
private:
	CImGuiMgr();
	virtual ~CImGuiMgr();
	friend class CSingleton<CImGuiMgr>;
};

