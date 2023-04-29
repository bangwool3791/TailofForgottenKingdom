#pragma once
#include "UI.h"

class CGameObjectEx;
class ComponentUI;
class ScriptUI;

#include <Engine\Ptr.h>
#include <Engine\CRes.h>
class ResUI;

class InspectorUI :
    public UI
{
private:
    vector<ScriptUI*>   m_vecScriptUI;

    CGameObjectEx*        m_TargetObj;
    ComponentUI*        m_arrComUI[(UINT)COMPONENT_TYPE::END];
    ComponentUI*        m_arrObjUI[(UINT)OBJECT_TYPE::END];

    Ptr<CRes>           m_TargetRes;
    ResUI*              m_arrResUI[(UINT)RES_TYPE::END];
public:

    void SetTargetObject(CGameObjectEx* _Target);
    void SetTargetResource(CRes* _Resource);
    void InitializeScriptUI();
    CGameObjectEx* GetTargetObject() { return m_TargetObj; }
    ScriptUI* GetScriptUI(const string& str);
private:
    virtual void update() override;
    virtual void render_update() override;

public:
    InspectorUI();
    ~InspectorUI();
};

