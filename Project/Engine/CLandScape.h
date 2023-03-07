#pragma once
#include "CRenderComponent.h"

class CLandScape :
    public CRenderComponent
{
private:
    UINT    m_iXFaceCount;
    UINT    m_iZFaceCount;

public:
    virtual void finaltick() override;
    virtual void render() override;
public:
    virtual void SaveToFile(FILE* _File) override {}
    virtual void LoadFromFile(FILE* _File) override {}
public:
    virtual void render_Instancing() override {}
public:
    void SetFaceCount(UINT _X, UINT _Z);


private:
    void CreateMesh();
    void CreateMaterial();


    CLONE(CLandScape);
public:
    CLandScape();
    ~CLandScape();
};

