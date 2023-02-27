#pragma once
#include <Engine\CScript.h>
class CCameraScript :
    public CScript
{
private:
    float   m_fSpeed;

public:
    virtual void begin() override;
    virtual void tick() override;

    virtual void BeginOverlap(CCollider* _pOther) {};
    virtual void Overlap(CCollider* _pOther) {};
    virtual void EndOverlap(CCollider* _pOther) {};

    void         Move();

public :
    CLONE(CCameraScript);
public:
    CCameraScript();
    virtual ~CCameraScript();

};

