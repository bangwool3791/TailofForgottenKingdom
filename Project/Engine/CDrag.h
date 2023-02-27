#pragma once

#include "CGameObject.h"

class CDrag
	:public CGameObject
{
	
public :
	virtual void begin();
	virtual void tick();
	virtual void finaltick();
	virtual void render();

public:
	CDrag();
	CDrag(const CDrag& rhs);
	virtual ~CDrag();
};