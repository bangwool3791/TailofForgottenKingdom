#pragma once
#include "ComponentUI.h"

class CGameObject;

class Light2DUI :
    public ComponentUI
{
private:
    CGameObject*            m_pDirectionalLight;
    CGameObject*            m_pPointLight;
    CGameObject*            m_pSpotLight;
    Vec3                    m_vColor;
    float                   m_fSpotLightRadius;
    float                   m_fSpotLightAngle;
    Vec3                    m_vSpotLightColor;

public :
    void SetDirectionalLight(CGameObject* _DirectionalLight){m_pDirectionalLight = _DirectionalLight;}
    void SetSpotLight(CGameObject* _SpotLight)
    { 
        if(!m_pSpotLight)
            m_pSpotLight = _SpotLight;
    }
private:
    virtual void update() override;
    virtual void render_update() override;

public:
    Light2DUI();
    ~Light2DUI();
};

