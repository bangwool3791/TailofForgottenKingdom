#pragma once
#include "ComponentUI.h"
#include "pch.h"

class CGameObject;

class PhysXComUI :
    public ComponentUI
{
private:

private:
    virtual void update() override;
    virtual void render_update() override;

public:
    PhysXComUI();
    ~PhysXComUI();
};

