#pragma once
#include "CRenderComponent.h"

#include "CRaycastShader.h"
#include "CHeightMapShader.h"

class CStructuredBuffer;

class CLandScape :
    public CRenderComponent
{
private:
    UINT                    m_iXFaceCount;
    UINT                    m_iZFaceCount;

    Vec2                    m_vBrushScale;

    Ptr<CRaycastShader>     m_pCSRaycast;   // ��ŷ ���̴�
    CStructuredBuffer*      m_pCrossBuffer;	// ���콺 ��ŷ�Ǵ� ���� ���� �޴� ����

    Ptr<CHeightMapShader>   m_pCSHeightMap; // ���̸� ���̴�
    Ptr<CTexture>           m_pBrushTex;    // �귯���� �ؽ���

    Ptr<CTexture>           m_pHeightMap;   // ���̸� �ؽ���
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
    void SaveTexture();
private:
    void CreateMesh();
    void CreateMaterial();
    void CreateTexture();
    void Raycasting();

    CLONE(CLandScape);
public:
    CLandScape();
    ~CLandScape();
};

