#pragma once
#include "ComponentUI.h"

#include <Engine\Ptr.h>
#include <Engine\CMesh.h>
#include <Engine\CMaterial.h>

class MeshRenderUI :
    public ComponentUI
{
private:
    Ptr<CMesh>          m_Mesh;
    Ptr<CMaterial>      m_Mtrl;


public:
    virtual void update() override;
    virtual void render_update() override;


private:
    void SetMesh(DWORD_PTR _strMeshKey);
    void SetMaterial(DWORD_PTR _strMaterialKey);


public:
    MeshRenderUI();
    ~MeshRenderUI();
};

