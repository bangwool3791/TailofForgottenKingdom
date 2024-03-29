#pragma once
#include "Waves.h"

class CCamera;

class CEngine
	: public CSingleton<CEngine>
{
private:
	HWND		m_hMainWnd;
	Vec2		m_vResolution;	// 윈도우 해상도

public:
	int init(HWND _hWnd, UINT _iWidth, UINT _iHeight);
	void progress();
private:
	void tick();
	void finaltick();
public:
	void render();
	void render(const vector<CGameObject*>& obj);
private:
	CEngine();
	virtual ~CEngine();
	friend class CSingleton<CEngine>;
public:
	HWND GetMainHwnd() { return m_hMainWnd; }
};

