#pragma once
#include "CRenderComponent.h"

#include "CRaycastShader.h"
#include "CHeightMapShader.h"
#include "CWeightMapShader.h"
#include "CShadowMapShader.h"

class CStructuredBuffer;

class CLandScape :
    public CRenderComponent
{
private:
    UINT                    m_iXFaceCount;
    UINT                    m_iZFaceCount;

    Vec2                    m_vBrushScale;

    Ptr<CRaycastShader>     m_pCSRaycast;   // 픽킹 쉐이더
    CStructuredBuffer* m_pCrossBuffer;	// 마우스 피킹되는 지점 정보 받는 버퍼

    Ptr<CHeightMapShader>   m_pCSHeightMap; // 높이맵 쉐이더
    Ptr<CTexture>           m_pBrushTex;    // 브러쉬용 텍스쳐

    Ptr<CTexture>           m_pTileArrTex;      // 타일 배열 텍스쳐

    Ptr<CTexture>           m_pHeightMap;   // 높이맵 텍스쳐

    Ptr<CWeightMapShader>   m_pCSWeightMap;     // 가중치 쉐이더
    Ptr<CShadowMapShader>   m_pCSShadowMap;     // 그림자 영역 9 픽셀 선별 쉐이더

    CStructuredBuffer* m_pWeightMapBuffer; // 가중치 저장 버퍼
    UINT					m_iWeightWidth;		// 가중치 버퍼 가로세로 행렬 수
    UINT					m_iWeightHeight;	// 가중치 버퍼 가로세로 행렬 수
    UINT                    m_iWeightIdx;		// 증가 시킬 가중치 부위

    LANDSCAPE_MOD           m_eMod; 	        // 지형 툴모드에서 상태값

    CGameObject* m_pCameraObj;

    Ptr<CMaterial>          m_LandScapeMtrl;
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

    void SetShadowMap(Ptr<CTexture> _pTex);
    void SetTargetMap(Ptr<CTexture> _pTex);

    void SetFaceCount(UINT _X, UINT _Z);
    void SetCameraObj(CGameObject* _pObj) { m_pCameraObj = _pObj; }
    void SaveTexture(const wstring& path);
    void SaveBmpFile(const wstring& path);
    void LoadBmpFile(const wstring& path);
    pair<UINT, UINT> GetFaceCount() { return pair<UINT, UINT>(m_iXFaceCount, m_iZFaceCount); }
    Vec2 GetBrushScale() { return m_vBrushScale; }
    void SetBrushScale(const Vec2& _vScale) { m_vBrushScale = _vScale; }
    template<typename T>
    void GetHeightTexture(T& t) { t = m_pHeightMap->GetSRV().Get(); }
    template<typename T>
    void GetBrushSRV(T& t) { t = m_pBrushTex->GetSRV().Get(); }
    Ptr<CTexture> GetBrushTexture() { return m_pBrushTex; }
    const tRaycastOut& GetRay();

    LANDSCAPE_MOD GetMode() { return m_eMod; }
    void SetMode(LANDSCAPE_MOD eType) { m_eMod = eType; }
private:
    void CreateMesh();
    void CreateMaterial();
    void Raycasting();
public:
    void CreateTexture();
    void Initialize();
    void SetHeightmapOffset(float _offset);
    void SetTexArr(Ptr<CTexture> _ptr);
    void SetDir(int _Dir) { m_pCSHeightMap->SetDir(_Dir); }
    CLONE(CLandScape);
public:
    CLandScape();
    ~CLandScape();
};

