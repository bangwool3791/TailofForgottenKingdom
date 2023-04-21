#include "pch.h"
#include "PhysXComUI.h"
#include <Engine\PhysXComponent.h>

PhysXComUI::PhysXComUI()
	: ComponentUI("PhysXComponent", COMPONENT_TYPE::PHYSX)
{
}

PhysXComUI::~PhysXComUI()
{
}


void PhysXComUI::update()
{

	ComponentUI::update();
}

void PhysXComUI::render_update()
{
	ComponentUI::render_update();
}