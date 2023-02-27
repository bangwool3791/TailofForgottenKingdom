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
	//�θ� ������Ʈ ���� vector
	vector<CGameObject*>			m_vecParent;
	//�θ�, �ڽ� ������Ʈ ���� vector, ������ ���� ������Ʈ�� �Ѱ��� ������, ���� �׷��� ������ ����
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