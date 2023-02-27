#pragma once

#include "UI.h"
#include <Engine\Ptr.h>
#include <Engine\CRes.h>

class ResUI :
    public UI
{
private:
    //현재 선택 된 리소스 
    Ptr<CRes>   m_TargetRes;

    RES_TYPE    m_eResType;

public:
    void SetTarget(Ptr<CRes> _pTarget) { m_TargetRes = _pTarget; }
    Ptr<CRes> GetTarget() { return m_TargetRes; }
    RES_TYPE GetComponentType() { return m_eResType; }

    virtual void render_update() override;

public:
    /*
    * 
    */
    ResUI(const string& _UIName, RES_TYPE _eType);
    ~ResUI();
};

