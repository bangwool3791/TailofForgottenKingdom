#pragma once
#include "UI.h"

class CComponent;
class ComponentUI;

#include <Engine\Ptr.h>
#include <Engine\CRes.h>

class ComInspector :
    public UI
{
private:
    CComponent*  m_TargetCom;
    ComponentUI* m_arrComUI[(UINT)COMPONENT_TYPE::END];
public:

    void SetTargetCom(CComponent* _Target);
private:
    virtual void update() override;
    virtual void render_update() override;

public:
    ComInspector();
    ~ComInspector();
};

