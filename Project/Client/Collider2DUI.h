#pragma once
#include "ComponentUI.h"
#include "pch.h"

class CGameObject;

class Collider2DUI :
    public ComponentUI
{
private:
    Vec2            m_vFinalPos;
    Vec2            m_vOffsetPos;
    Vec2            m_vScale;
    Vec3            m_vRot;
    bool            m_bIgnorObjectScale;

private:
    virtual void update() override;
    virtual void render_update() override;

public:
    Collider2DUI();
    ~Collider2DUI();
};

