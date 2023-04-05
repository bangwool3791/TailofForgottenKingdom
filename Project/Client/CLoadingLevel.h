#pragma once

#include "CEditor.h"
#include "CreateTestLevel.h"
#include "CSaveLoadMgr.h"
#include "CImGuiMgr.h"

class CCamera;
class CLoader;
class CLoadingLevel
	: public CSingleton<CLoadingLevel>
{
private:
	CLoader*			m_pLoader;
	CCamera*		    m_pCameraLoader;
	CGameObject*		m_pLightObj;
	CGameObject*		m_pLoderCamearObj;
	CGameObject*		m_pLoderUIObj;
	vector<CGameObject*> m_vecEditoShadowObj;
private:
	void CreateUI();
public:
	void init();
	void progress();
	void render();
private:
	CLoadingLevel();
	virtual ~CLoadingLevel();
	friend class CSingleton<CLoadingLevel>;
};