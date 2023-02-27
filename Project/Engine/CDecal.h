#pragma once
#include "CRenderComponent.h"

class CDecal :
    public CRenderComponent
{
private:
    Ptr<CTexture>   m_DecalTex;
    bool            m_bLighting;


public:
    void SetDefaultLit(bool _bSet);
    void SetDecalTexture(Ptr<CTexture> _tex) { m_DecalTex = _tex; }


public:
    virtual void finaltick() override;
public:
    virtual void SaveToFile(FILE* _File) override ;
    virtual void LoadFromFile(FILE* _File) override;
public:
    virtual void render();
    virtual void render_Instancing();
public:
    CLONE(CDecal);
public:
    CDecal();
    CDecal(const CDecal& _rhs);
    ~CDecal();
};

