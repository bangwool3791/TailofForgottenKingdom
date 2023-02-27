#pragma once
#include "CScript.h"

class CAniPoint :
    public CScript
    //, public SmallObjAllocator<CAniPoint, 1>
{
public:
    virtual void begin();
    virtual void tick();
    virtual void finaltick();

    virtual void BeginOverlap(CCollider2D* _pOther);
    virtual void Overlap(CCollider2D* _pOther);
    virtual void EndOverlap(CCollider2D* _pOther);

    CLONE(CAniPoint);

private:
    bool binit;
    bool bClicked;
    Vec2 StartPos;
    Vec2 EndPos;

    CGameObject* m_pCamera;
    float                   m_fCameraScale;
    Vec2                    m_vCameraPos;
    Vec2                    m_vRenderResolution;
    Vec2                    m_vMousePos;
    Vec3                    m_vTarget;
    Vec3                    m_vPos;
public:
    CAniPoint();
    virtual ~CAniPoint();

};

