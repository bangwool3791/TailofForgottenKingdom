#pragma once
#include <Engine/CGameObject.h>
#include <Engine/smallobjallocator.h>

class CGameObjectEx
	:public CGameObject
{
public :
	virtual void finaltick();

	CLONE(CGameObjectEx);
public :
	CGameObjectEx();
	CGameObjectEx(const CGameObject& rhs);
	virtual ~CGameObjectEx();
};