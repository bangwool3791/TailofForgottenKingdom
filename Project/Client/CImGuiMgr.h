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
public:
	UI* FindUI(const string& _name);

public:	
	void init(HWND _hWnd);
	void progress();
	void Guizmo();
	void SetPickingObj(CGameObjectEx* pObj) { m_pSelectedObj = pObj; }
private:
	void clear();
	void CreateUI();
	void ObserveContent();
private:
	CImGuiMgr();
	virtual ~CImGuiMgr();
	friend class CSingleton<CImGuiMgr>;
};

