#pragma once

class CLevel;

class CLevelMgr
	:public CSingleton<CLevelMgr>
{
private:
	CLevel* m_pCurLevel;

public:
	CLevel* GetCurLevel()
	{
		return m_pCurLevel;
	}
	CGameObject* FindObjectByName(const wstring& _strName);
	CGameObject* FindSelectedObject(const wstring& _strName);
public:
	void init();
	void progress();
	void render();
	void ChangeLevel(CLevel* _NextLevel);
	void ChangeLevelState(LEVEL_STATE _State);
	LEVEL_STATE GetLevelState();
private:
	CLevelMgr();
	virtual ~CLevelMgr();
	friend class CSingleton<CLevelMgr>;
};

