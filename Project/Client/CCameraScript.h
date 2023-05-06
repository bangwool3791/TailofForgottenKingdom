#pragma once
#include <Engine\CScript.h>

enum class CamProc
{
    STEP1,
    STEP2,
    STEP3,
    END
};
class CCameraScript :
    public CScript
{
private:
    bool    m_bJump;
    CamProc m_eProc;
    float   m_fSpeed;
    float   m_fDistance;
    float   m_fOffset = 0.f;
    Vec3    m_vRot;
    CGameObjectEx* m_pPlayer = nullptr;
public:
    virtual void begin() override;
    virtual void tick() override;
    virtual void finaltick() override;

    virtual void BeginOverlap(CCollider* _pOther) {};
    virtual void Overlap(CCollider* _pOther) {};
    virtual void EndOverlap(CCollider* _pOther) {};

    void         Move();
    void         EditMode();
    void         SetPlayer(CGameObjectEx* pPlayer);
public :
    CLONE(CCameraScript);
public:
    CCameraScript();
    virtual ~CCameraScript();

};

