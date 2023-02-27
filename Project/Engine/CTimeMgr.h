#pragma once

class CTimeMgr : 
	public CSingleton<CTimeMgr>
{
private:
	LARGE_INTEGER	m_llFrequence;
	LARGE_INTEGER	m_llCurCount;
	LARGE_INTEGER   m_llPrevCount;

	float			m_fAccTime;
	float			m_fDeltaTime;

public:
	void init();
	void tick();
	void render();

public:
	float GetDeltaTime() { return m_fDeltaTime; }
private:
	CTimeMgr();
	virtual ~CTimeMgr();
	friend class CSingleton<CTimeMgr>;
};

