#pragma once
#include <Engine\CScript.h>
class CSecondCameraScript :
    public CScript
{
private:
    float   m_fSpeed;
    float   m_fScale;
    Vec3    m_vPos;
public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider* _pOther) {};
    virtual void Overlap(CCollider* _pOther) {};
    virtual void EndOverlap(CCollider* _pOther) {};

    void         Move();

public:
    CLONE(CSecondCameraScript);
public:
    CSecondCameraScript();
    virtual ~CSecondCameraScript();

};

