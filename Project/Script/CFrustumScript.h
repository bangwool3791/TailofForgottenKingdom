#pragma once
#include <Engine\CScript.h>

class CCollider;

class CFrustumScript :
    public CScript
{
private:
    CCamera* m_pCamera;
    CFrustum* m_pFrustum;
private:
    void SaveToFile(FILE* _File);
    void LoadFromFile(FILE* _File);
public:
    virtual void begin();
    virtual void tick();
    virtual void BeginOverlap(CCollider* _pOther);
    virtual void Overlap(CCollider* _pOther);
    virtual void EndOverlap(CCollider* _pOther);
    CLONE(CFrustumScript);
public:
    CFrustumScript();
    virtual ~CFrustumScript();
};

