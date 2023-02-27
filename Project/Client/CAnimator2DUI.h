#pragma once
#include "ComponentUI.h"
#include "pch.h"

class CGameObject;

class CAnimator2DUI :
    public ComponentUI
{
private:
    string              m_strTexName;
    string              m_CurKey;
    vector<string>      m_vecKey;
    int                 m_iCurIndex;

private:
    virtual void update() override;
    virtual void render_update() override;

public:
    CAnimator2DUI();
    ~CAnimator2DUI();
};

