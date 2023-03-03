#pragma once
#include "CRenderComponent.h"

#include "CSLight.h"

class CStructuredBuffer;

class CLightSystem :
    public CRenderComponent
{
private:
    int                         m_WorldSpawn;

    CStructuredBuffer* m_ParticleBuffer;
    CStructuredBuffer* m_ParticleShare;
    Ptr<CSLight>  m_UpdateCS;


public:
    void SetWorldSpawn(bool _bSet) { m_WorldSpawn = (int)_bSet; }


public:
    virtual void finaltick() override;
    virtual void render() override;


public:
    virtual void SaveToFile(FILE* _File) override {};
    virtual void LoadFromFile(FILE* _File) override {};
    CLONE(CLightSystem);
public:
    CLightSystem();
    ~CLightSystem();
};

