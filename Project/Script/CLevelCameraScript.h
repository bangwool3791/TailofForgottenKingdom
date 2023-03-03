#pragma once
#include <Engine\CScript.h>
class CLevelCameraScript :
    public CScript
{
private:
    CCamera*  m_pCamera;
    CFrustum* m_pFrustum;
    CGameObject* m_pObjFrustum;
private:
    float   m_fSpeed;
    float   m_fScale;
    float   m_fFOV;
    float   m_fFar;
    Vec3    m_vPos;
public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider* _pOther) {};
    virtual void Overlap(CCollider* _pOther) {};
    virtual void EndOverlap(CCollider* _pOther) {};

    void         Move();

public:
    CLONE(CLevelCameraScript);
public:
    CLevelCameraScript();
    virtual ~CLevelCameraScript();

};

