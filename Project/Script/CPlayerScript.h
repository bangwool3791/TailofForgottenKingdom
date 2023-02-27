#pragma once
#include <Engine\CScript.h>

class CCollider;

class CPlayerScript :
    public CScript
{
private:
    float        m_fAccTime;
    float        m_fSpeed;
    float        m_fJumpHeight;
    Ptr<CPrefab> m_Prefab;
private:
    void SaveToFile(FILE* _File);
    void LoadFromFile(FILE* _File);
public :
    virtual void begin();
    virtual void tick();
    virtual void BeginOverlap(CCollider* _pOther);
    virtual void Overlap(CCollider* _pOther);
    virtual void EndOverlap(CCollider* _pOther);
    CLONE(CPlayerScript);
public:
    CPlayerScript();
    virtual ~CPlayerScript();
};

