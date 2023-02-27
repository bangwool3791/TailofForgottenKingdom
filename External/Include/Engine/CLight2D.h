#pragma once
#include "CComponent.h"

class CLight2D :
	public CComponent
{
private:
	tLightInfo	m_Info;

public :
	const tLightInfo& GetLightInfo() { return m_Info; }

    void SetLightColor(Vec3 _vColor) { m_Info.vDiff = _vColor; }
    void SetLightType(LIGHT_TYPE _eType) { m_Info.iLightType = _eType; }
    void SetRadius(float _fRadius) { m_Info.fRadius = _fRadius; }
    void SetAngle(float _fAngle) { m_Info.fAngle = _fAngle; }

    Vec3 GetLightColor() { return m_Info.vDiff; }
    LIGHT_TYPE GetLightType(LIGHT_TYPE _eType) { return m_Info.iLightType; }
    float GetRadius()
    { 
        return m_Info.fRadius;
    }
    float GetAngle()
    { 
        return m_Info.fAngle; 
    }

public :
    virtual void finaltick() override;

public:
    virtual void SaveToFile(FILE* _File) override;
    virtual void LoadFromFile(FILE* _File) override;
    CLONE(CLight2D);

public:
    CLight2D();
    virtual ~CLight2D();
};