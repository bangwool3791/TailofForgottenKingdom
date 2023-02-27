#pragma once
#include "CEntity.h"

enum class FACE_TYPE
{
    FT_NEAR,
    FT_FAR,
    FT_UP,
    FT_DOWN,
    FT_LEFT,
    FT_RIGHT,

    END,
};

class CCamera;

class CFrustum :
    public CEntity
{
private:
    CCamera* m_Owner;
    Vec3        m_vPos;
    Vec3        m_ProjPos[8];
    Vec3        m_WorldPos[8];
    int         m_iSize = 0;
    Vec4        m_Plane[(UINT)FACE_TYPE::END];



public:
    void finaltick();
    bool CheckFrustum(Vec3 _vPos);
    bool CheckFrustumRadius(Vec3 _vPos, float _fRadius);

    Vec3 GetPosition() { return m_vPos; }
    Vec3* GetWorldArray(int& _iSize) 
    { 
        _iSize = m_iSize;
        return m_WorldPos; 
    }
    CLONE(CFrustum);
public:
    CFrustum(CCamera* _Owner);
    ~CFrustum();
};

