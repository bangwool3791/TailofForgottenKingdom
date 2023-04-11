#pragma once
#include "CRenderComponent.h"

#include "Waves.h"

class CWaveRenderer :
    public CRenderComponent
{
private:
    Waves            m_Wave;
    UINT             m_iXFaceCount;
    UINT             m_iZFaceCount;
private:
    void CreateMesh();
    void CreateMaterial();
public:
    void Initialize();
public:
    virtual void tick() override;
    virtual void finaltick() override;
    virtual void render() override;
    virtual void render(UINT _iSubset) override;
public:
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;

    CLONE(CWaveRenderer);
public:
    CWaveRenderer();
    ~CWaveRenderer();
};