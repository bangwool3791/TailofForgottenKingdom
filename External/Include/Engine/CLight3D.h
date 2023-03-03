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
    tLightInfo      m_Info; // ±¤¿ø Á¤º¸
    Ptr<CMesh>      m_pVolumeMesh;
    Ptr<CMaterial>  m_pLightMtrl;

    int             m_iLightIdx;    // ±¤¿ø ¹öÆÛ¿¡¼­ÀÇ ÀÎµ¦½º
public:
    const tLightInfo& GetLightInfo() { return m_Info; }

    void SetLightColor(Vec3 _vColor) { m_Info.vDiff = _vColor; }
    void SetLightSpecular(Vec3 _vSpec) { m_Info.vSpec = _vSpec; }
    void SetLightAmbient(Vec3 _vAmb) { m_Info.vAmb = _vAmb; }
    void SetLightType(LIGHT_TYPE _eType);

    void SetRadius(float _fRadius) { m_Info.fRadius = _fRadius; }
    void SetAngle(float _fAngle) { m_Info.fAngle = _fAngle; }

    Vec3 GetLightColor() { return m_Info.vDiff; }
    LIGHT_TYPE GetLightType(LIGHT_TYPE _eType) { return m_Info.iLightType; }
    float GetRadius(float _fRadius) { return m_Info.fRadius; }
    float GetAngle(float _fAngle) { return m_Info.fAngle; }


public:
    virtual void finaltick() override;
    void render();
public:
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;

    CLONE(CLight3D);
public:
    CLight3D();
    ~CLight3D();
};

