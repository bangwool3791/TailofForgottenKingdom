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

    Ptr<CRaycastShader>     m_pCSRaycast;   // ��ŷ ���̴�
    CStructuredBuffer* m_pCrossBuffer;	// ���콺 ��ŷ�Ǵ� ���� ���� �޴� ����

    Ptr<CHeightMapShader>   m_pCSHeightMap; // ���̸� ���̴�
    Ptr<CTexture>           m_pBrushTex;    // �귯���� �ؽ���

    Ptr<CTexture>           m_pTileArrTex;      // Ÿ�� �迭 �ؽ���

    Ptr<CTexture>           m_pHeightMap;   // ���̸� �ؽ���

    Ptr<CWeightMapShader>   m_pCSWeightMap;     // ����ġ ���̴�
    Ptr<CShadowMapShader>   m_pCSShadowMap;     // �׸��� ���� 9 �ȼ� ���� ���̴�

    CStructuredBuffer* m_pWeightMapBuffer; // ����ġ ���� ����
    UINT					m_iWeightWidth;		// ����ġ ���� ���μ��� ��� ��
    UINT					m_iWeightHeight;	// ����ġ ���� ���μ��� ��� ��
    UINT                    m_iWeightIdx;		// ���� ��ų ����ġ ����

    LANDSCAPE_MOD           m_eMod; 	        // ���� ����忡�� ���°�

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

