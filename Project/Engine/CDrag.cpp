
#include "pch.h"

#include "CDrag.h"
#include "CGameObject.h"

CDrag::CDrag()
{

}
CDrag::CDrag(const CDrag& rhs)
	:CGameObject(rhs)
{

}

CDrag::~CDrag()
{
}

void CDrag::begin()
{
	CGameObject::begin();
}

void CDrag::tick()
{
	CGameObject::tick();
}

void CDrag::finaltick()
{
	CGameObject::finaltick();
}

void CDrag::render()
{
	CGameObject::render();
}

