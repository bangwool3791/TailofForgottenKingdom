#pragma once
#include "ComponentUI.h"
#include "pch.h"

class CGameObject;

class TrailComUI :
    public ComponentUI
{
private:
    int m_iBoneIdx = 0;
private:
    virtual void update() override;
    virtual void render_update() override;

public:
    TrailComUI();
    ~TrailComUI();
};

