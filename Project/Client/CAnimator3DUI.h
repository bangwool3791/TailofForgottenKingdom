#pragma once
#include "ComponentUI.h"
#include "pch.h"

class CGameObject;

class CAnimator3DUI :
    public ComponentUI
{

private:
    virtual void update() override;
    virtual void render_update() override;

public:
    CAnimator3DUI();
    ~CAnimator3DUI();
};

