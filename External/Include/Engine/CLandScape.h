#pragma once
#include "CRenderComponent.h"

#include "CRaycastShader.h"
#include "CHeightMapShader.h"

class CStructuredBuffer;

class CLandScape :
    public CRenderComponent
{
private:
    bool                    m_bCreateTex;
    UINT                    m_iXFaceCount;
    UINT                    m_iZFaceCount;

    Vec2                    m_vBrushScale;

    Ptr<CRaycastShader>     m_pCSRaycast;   // 픽킹 쉐이더
    CStructuredBuffer* m_pCrossBuffer;	// 마우스 피킹되는 지점 정보 받는 버퍼

    Ptr<CHeightMapShader>   m_pCSHeightMap; // 높이맵 쉐이더
    Ptr<CTexture>           m_pBrushTex;    // 브러쉬용 텍스쳐

    Ptr<CTexture>           m_pHeightMap;   // 높이맵 텍스쳐
private:
    void Update_HeightMap();
public:
    virtual void finaltick() override;
    virtual void render() override;
public:
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;
public:
    virtual void render_Instancing() override {}
public:
    void SetBrushMap(Ptr<CTexture> _pTex);
    void SetHeightMap(Ptr<CTexture> _pTex);
    void SetFaceCount(UINT _X, UINT _Z);
    void SaveTexture(const wstring& path);
    void SaveBmpFile(const wstring& path);
    void LoadBmpFile(const wstring& path);
    pair<UINT, UINT> GetFaceCount() { return pair<UINT, UINT>(m_iXFaceCount, m_iZFaceCount); }
    template<typename T>
    void GetHeightTexture(T& t) { t = m_pHeightMap->GetSRV().Get(); }
    template<typename T>
    void GetBrushTexture(T& t) { t = m_pBrushTex->GetSRV().Get(); }
private:
    void CreateMesh();
    void CreateMaterial();
    void Raycasting();
public:
    void CreateTexture();
    void Initialize();

    CLONE(CLandScape);
public:
    CLandScape();
    ~CLandScape();
};

