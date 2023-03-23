#pragma once
#include <Engine\CScript.h>

class CCollider;

class CEngineBrushScript :
    public CScript
{
private:
    void SaveToFile(FILE* _File);
    void LoadFromFile(FILE* _File);
public:
    virtual void begin();
    virtual void tick();
    virtual void finaltick();
    virtual void BeginOverlap(CCollider* _pOther);
    virtual void Overlap(CCollider* _pOther);
    virtual void EndOverlap(CCollider* _pOther);
    CLONE(CEngineBrushScript);
public:
    CEngineBrushScript();
    virtual ~CEngineBrushScript();
};

