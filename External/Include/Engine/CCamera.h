#pragma once
#include "CComponent.h"

#include "CFrustum.h"

class CSLight;

class CStructuredBuffer;
enum PROJ_TYPE
{
    PERSPECTIVE,
    ORTHOGRAHPIC,
};

class CCamera :
    public CComponent

{
private:
    tRay                    m_ray;      // 마우스 방향을 향하는 직선
    CSLight* m_LightCS;
    CFrustum                m_Frustum;

    Matrix                  m_matView;
    Matrix                  m_matViewInv;
    Matrix                  m_matProj;
    Matrix                  m_matProjInv;
    Matrix                  m_matReflect;

    Matrix                  m_matCubeMapView[6];
    Matrix                  m_matCubeMapProj;

    D3D11_VIEWPORT			m_tEnvViewPort[1];			// 환경맵용 
    D3D11_VIEWPORT          m_tViewPort;
    PROJ_TYPE               m_eProjType;
    float                   m_fWidth;       // 투영 가로길이
    float                   m_fAspectRatio; // 종횡 비 (가로길이 / 세로길이)

    float                   m_FOV;          // 시야각

    double                   m_fFar;
    double                   m_fNear;
    double                   m_fScale;

    UINT                    m_iLayerMask;
    int                     m_iCamIdx;

    vector<CGameObject*>                     m_vecDeferred;
    vector<CGameObject* >                    m_vecOpaque;
    vector<CGameObject* >                    m_vecMask;
    vector<CGameObject*>                     m_vecDecal;
    vector<CGameObject* >                    m_vecTransparent;
    vector<CGameObject*>                     m_vecDynamicShadow;     // 동적 그림자 물체
    map<const wstring, vector<CGameObject*>> m_mapOpaqueVec;
    map<const wstring, vector<CGameObject*>> m_mapMaskVec;
    map<const wstring, vector<CGameObject*>> m_mapDecalVec;

    map<const wstring, vector<CGameObject*>> m_mapTransparentVec;
    vector<CGameObject* >                    m_vecPostProcess;
    vector<CGameObject* >                    m_vecUi;

    CStructuredBuffer* m_pObjectRenderBuffer;

private:
    void begin();
    void SortObject();
    void render_deferred();
    void render_opaque();
    void render_mask();
    void render_decal();
    void render_transparent();
    void render_postprocess();
    void render_ui();
    //void update_render(Ptr<CMesh> p);
public:
    void            InitializeEnvView(Vec3 vEyePos);
    virtual void    finaltick();
    virtual void    finaltick_module();
    void            render();
    void            EditorRender();
    void            render(MRT_TYPE _eType);
    void            SortObject(const vector<CGameObject*>& vec);
    void            SortShadowObject();
    void            SortShadowObject(const vector<CGameObject*>& vec);
    void            render_depthmap();
public:
    void CalcViewMat();
    void CalcProjMat();
    void CalcReflectMat(float height);
public:

    double          GetOrthographicScale() { return m_fScale; }
    double& GetOrthographicScale_() { return m_fScale; }
    void            SetOrthographicScale(float _fScale) { m_fScale = _fScale; }

    void            SetProjType(PROJ_TYPE _eType) { m_eProjType = _eType; }
    PROJ_TYPE       GetProjType() { return m_eProjType; }

    void            SetAspectRatio(float _fRatio) { m_fAspectRatio = _fRatio; }
    float           GetAspectRatio() { return m_fAspectRatio; }

    void            SetNear(double _fNear) { m_fNear = _fNear; }
    double          GetNear() { return m_fNear; }
    void            SetFar(double _fFar) { m_fFar = _fFar; }
    double          GetFar() { return m_fFar; }

    void        CalRay();
    const tRay& GetRay() { return m_ray; }
    float GetFOV() { return m_FOV; }
    void SetFOV(float _fov) { m_FOV = _fov; }
    void SetWidth(float _fWidth) { m_fWidth = _fWidth; }
    /*
    * Matrix객체는 const ref로 return
    */
    const Matrix& GetViewMat() { return m_matView; }
    const Matrix& GetViewMatInv() { return m_matViewInv; }
    const Matrix& GetProjMat() { return m_matProj; }
    const Matrix& GetProjMatInv() { return m_matProjInv; }
    void SetLayerMask(const wstring& _strLayerName);
    void SetLayerMask(int _iLayerIdx);
    void SetLayerMaskAll() { m_iLayerMask = 0xffffffff; }
    void SetLayerMaskZero() { m_iLayerMask = 0; }
    CFrustum* GetFrustum() { return &m_Frustum; }
public:
    CLONE(CCamera);

public:
    virtual void SaveToFile(FILE* _File);
    virtual void LoadFromFile(FILE* _File);
public:
    CCamera();
    CCamera(const CCamera& rhs);
    ~CCamera();
};

