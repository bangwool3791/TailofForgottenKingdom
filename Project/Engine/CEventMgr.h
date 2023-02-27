#pragma once

class CGameObject;

class CEventMgr :
	public CSingleton<CEventMgr>
{
private:
	vector<tEvent>			m_vecEvent;
	vector<CGameObject*>	m_vecGarbage;

	bool					m_bLevelChanged;
public :
	void tick();
public:
	void AddEvent(tEvent& _evn) { m_vecEvent.push_back(_evn); }
	bool IsLevelChanged() {return m_bLevelChanged;}
private:
	CEventMgr();
	virtual ~CEventMgr();
	friend class CSingleton<CEventMgr>;
};