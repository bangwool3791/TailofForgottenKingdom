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
	virtual ~CGameObjectEx();
};