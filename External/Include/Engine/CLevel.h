#pragma once

#include "CLayer.h"

class CLevel 
	: public CEntity
{
private:
	array<CLayer, MAX_LAYER>		m_arrLayer;
	LEVEL_STATE						m_State;
public :
	void begin();
	void tick();
	void finaltick();
	void render();
	void ClearLayer();
public:
	LEVEL_STATE GetState() { return m_State; }
	void SetState(LEVEL_STATE _state) { m_State = _state; }
	CLayer* GetLayer(UINT _iIdx) { return &m_arrLayer[_iIdx]; }
	CLayer* GetLayer(const wstring& _strLayerName);
	CGameObject* FindObjectByName(const wstring& _Name);
	CGameObject* FindSelectedObject(const wstring& _Name);
	CGameObject* FindParentObjectByName(const wstring& _Name);
	void AddGameObject(CGameObject* _pObject, UINT _iLayerIdx);
	void AddGameObject(CGameObject* _pObject, const wstring& _strLayerName);

	CLONE_DUMMY(CLevel);
public:
	CLevel();
	virtual ~CLevel();
};