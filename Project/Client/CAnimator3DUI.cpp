#include "pch.h"
#include "CAnimator3DUI.h"

#include <Engine/CGameObject.h>
#include <Engine\CAnimator3D.h>

CAnimator3DUI::CAnimator3DUI()
	: ComponentUI("Animator3D", COMPONENT_TYPE::ANIMATOR3D)
{
}

CAnimator3DUI::~CAnimator3DUI()
{
}


void CAnimator3DUI::update()
{
	ComponentUI::update();
}

void CAnimator3DUI::render_update()
{
	ComponentUI::render_update();
}