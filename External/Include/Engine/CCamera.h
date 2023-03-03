#pragma once
#include "CComponent.h"

#include "CFrustum.h"

class CSLight;

class CStructuredBuffer;
enum PROJ_TYPE
{
    PERSPECTIVE,
    ORTHOGRAHPICS,
};

class CCamera :
    public CComponent

{
private:
    CSLight*                m_LightCS;
    CFrustum                m_Frustum;
    Ray                     m_ray;

    Matrix                  m_matView;
    Matrix                  m_matViewInv;
    Matrix                  m_matProj;
    Matrix                  m_matProjInv;

    PROJ_TYPE               m_eProjType;
    float                   m_fAspectRatio;

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
    vector<CGameObject* >                    m_vecSound;
    map<const wstring, vector<CGameObject*>> m_mapOpaqueVec;
    map<const wstring, vector<CGameObject*>> m_mapMaskVec;
    map<const wstring, vector<CGameObject*>> m_mapDecalVec;

    map<const wstring, vector<CGameObject*>> m_mapTransparentVec;
    vector<CGameObject* >                    m_vecPostProcess;

    CStructuredBuffer*                       m_pObjectRenderBuffer;
private:
    void begin();
private :
    void SortObject();
    void render_deferred();
    void render_opaque();
    void render_mask();
    void render_decal();
    void render_transparent();
    void render_postprocess();
    void render_ui() {}
    void Sound();
    //void update_render(Ptr<CMesh> p);
public :
    virtual void finaltick();
    void         render();
public:
    void CalcViewMat();
    void CalcProjMat();
public :

    double        GetOrthographicScale() { return m_fScale; }
    double&       GetOrthographicScale_() { return m_fScale; }
    void         SetOrthographicScale(float _fScale) { m_fScale = _fScale; }

    void        SetProjType(PROJ_TYPE _eType) { m_eProjType = _eType; }
    PROJ_TYPE   GetProjType() { return m_eProjType; }

    void        SetAspectRatio(float _fRatio) { m_fAspectRatio = _fRatio; }
    float       GetAspectRatio() { return m_fAspectRatio; }

    void        SetNear(double _fNear) { m_fNear = _fNear; }
    double       GetNear() { return m_fNear; }
    void        SetFar(double _fFar) { m_fFar = _fFar; }
    double       GetFar() { return m_fFar; }

    Ray        CalRay();

    float GetFOV() { return m_FOV; }
    void SetFOV(float _fov) { m_FOV = _fov; }
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
public :
    CLONE(CCamera);

public:
    virtual void SaveToFile(FILE* _File);
    virtual void LoadFromFile(FILE* _File);
public :
    CCamera();
    CCamera(const CCamera& rhs);
    ~CCamera();
};

