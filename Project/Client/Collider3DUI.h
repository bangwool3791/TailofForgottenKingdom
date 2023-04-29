#pragma once
#include "ComponentUI.h"
#include "pch.h"

class CGameObject;

class Collider3DUI :
    public ComponentUI
{
private:
    Vec3            m_vFinalPos;
    Vec3            m_vOffsetPos;
    Vec3            m_vScale;
    Vec3            m_vRot;
    Vec3            m_vRotOffset;
    bool            m_bIgnorObjectScale;
    int             m_iBoneIdx = 0;
private:
    virtual void update() override;
    virtual void render_update() override;

public:
    Collider3DUI();
    ~Collider3DUI();
};

