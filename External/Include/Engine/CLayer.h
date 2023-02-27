#pragma once

#include "CEntity.h"

class CGameObject;

class CLayer
	:public CEntity
	
{
public :
	CLayer();
	~CLayer();
public :
	void begin();
	void tick();
	void finaltick();
	void render();

private :
	//부모 오브젝트 보관 vector
	vector<CGameObject*>			m_vecParent;
	//부모, 자식 오브젝트 보관 vector, 랜더시 게임 오브젝트를 한곳에 모으고, 랜더 그룹을 나누기 위함
	vector<CGameObject*>			m_vecObjects;
	int								m_iLayerIdx;

public :
	const vector<CGameObject*>& GetParentObjects() { return m_vecParent; }
	const vector<CGameObject*>& GetObjects() { return m_vecObjects; }
	CGameObject* FindParent(const wstring& _name);
	void RegisterObject(auto _pObj) { m_vecObjects.push_back(_pObj); }
	void clear()					{ m_vecObjects.clear(); }
	CLONE_DUMMY(CLayer);
public :
	void AddGameObject(CGameObject*_pObject);
	int	 GetLayerIndex()			{ return m_iLayerIdx; }
	void DeregisterObject(CGameObject* _pObj);
	friend class CLevel;
	friend class CGameObject;

};