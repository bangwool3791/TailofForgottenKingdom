#include "pch.h"
#include "CUIMgr.h"

#include "CGameObject.h"

CUIMgr::CUIMgr()
	:m_arrUi{}
{
}

CUIMgr::~CUIMgr()
{

}


void CUIMgr::AddUI(CGameObject* _pObj, UI_TYPE _eType)
{
	m_arrUi[(UINT)(_eType)].push_back(_pObj);
}

void CUIMgr::DeleteUI(UI_TYPE _eType)
{
	const vector<CGameObject*>& GameObjects = m_arrUi[(UINT)_eType];
	for (auto iter{ GameObjects.begin() }; iter != GameObjects.end(); ++iter)
	{
		(*iter)->Destroy();
	}
	m_arrUi[(UINT)_eType].clear();
}

