#pragma once

class CGameObject;

class CInterfaceMgr :
	public CSingleton<CInterfaceMgr>
{
private:
	CGameObject* m_pTarget;
	CGameObject* m_pBuildObj;
	vector<CGameObject*>		   m_vecTapButton;
public:
	const vector<CGameObject*>& GetTapButtons() { return m_vecTapButton; }
	CGameObject* GetBtn(UINT idx) { return m_vecTapButton[idx]; }
	CGameObject* GetBuildObj() { return m_pBuildObj; }
	void SetBuildObj(CGameObject* _pGameObject) { m_pBuildObj = _pGameObject; }
	CGameObject* GetTarget();
	void SetTarget(CGameObject* _pGameObject) { m_pTarget = _pGameObject; }
	void AddTapButton(CGameObject* _pGameObject);
private:
	CInterfaceMgr();
	virtual ~CInterfaceMgr();
	friend class CSingleton<CInterfaceMgr>;
};