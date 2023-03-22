#pragma once
#include "UI.h"
#include "TreeUI.h"

#include <Engine\CGameObject.h>

class ResUI;

class PopupUI :
    public UI
{
private:
    TreeUI* m_TreeUI;
    CGameObject* m_TargetObj;
public:

    void SetTargetObject(CGameObject* _Target);
    void SetTargetResource(CRes* _Resource);

    CGameObject* GetTargetObject() { return m_TargetObj; }
private:
    virtual void update() override;
    virtual void render_update() override;

public:
    PopupUI();
    ~PopupUI();
};

