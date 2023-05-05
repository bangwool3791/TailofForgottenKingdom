#pragma once
#include "CComponent.h"

#include "Ptr.h"
#include "CMesh.h"
#include "CSLight.h"

class CStructuredBuffer;


class CLight3D :
    public CComponent
{
private:
    tLightInfo      m_Info; // 광원 정보
    Ptr<CMesh>      m_pVolumeMesh;
    Ptr<CMaterial>  m_pLightMtrl;

    int             m_iLightIdx;    // 광원 버퍼에서의 인덱스

    CGameObject* m_pLightCam;    // 광원 시점용 카메라
public:
    const tLightInfo& GetLightInfo() { return m_Info; }

    void SetLightColor(Vec3 _vColor) { m_Info.vDiff = _vColor; }
    void SetLightSpecular(Vec3 _vSpec) { m_Info.vSpec = _vSpec; }
    void SetLightAmbient(Vec3 _vAmb) { m_Info.vAmb = _vAmb; }
    void SetLightType(LIGHT_TYPE _eType);
    void SetLightDirection(Vec3 _vDir);

    void SetRadius(float _fRadius) { m_Info.fRadius = _fRadius; }
    void SetAngle(float _fAngle) { m_Info.fAngle = _fAngle; }

    Vec3 GetLightColor() { return m_Info.vDiff; }
    LIGHT_TYPE GetLightType() { return m_Info.iLightType; }
    float GetRadius(float _fRadius) { return m_Info.fRadius; }
    float GetAngle(float _fAngle) { return m_Info.fAngle; }

public:
    virtual void finaltick() override;
    void render();
    void render_static_depthmap();
    void render_static_depthmap(const vector<CGameObject*>& obj);

    void render_dynamic_depthmap();
    void render_dynamic_depthmap(const vector<CGameObject*>& obj);
public:
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;

    CLONE(CLight3D);
public:
    CLight3D();
    CLight3D(const CLight3D& _origin);
    ~CLight3D();
};

