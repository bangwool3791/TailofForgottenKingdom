#pragma once
#include "UI.h"


class ComponentUI :
    public UI
{
private:
    CGameObject*    m_TargetObj;
    COMPONENT_TYPE  m_eType = (COMPONENT_TYPE)- 1;
    OBJECT_TYPE     m_eObjType = (OBJECT_TYPE)-1;
public:
    void SetTarget(CGameObject* _pTarget) { m_TargetObj = _pTarget; }
    CGameObject* GetTarget() { return m_TargetObj; }
    COMPONENT_TYPE GetComponentType() { return m_eType; }

    virtual void update() override {};
    virtual void render_update() override;

public:
    ComponentUI(const string& _UIName, COMPONENT_TYPE _eType);
    ComponentUI(const string& _UIName, OBJECT_TYPE _eType);
    ~ComponentUI();
};

