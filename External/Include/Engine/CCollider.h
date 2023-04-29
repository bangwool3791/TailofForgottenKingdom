#pragma once
#include "CComponent.h"

class CCollider
    :public CComponent
{
protected:
    //Common Value
    Vec3            m_vRot;
    Matrix          m_matWorld;
    bool            m_bIgnorObjectScale;
    bool            m_bPause;
    int             m_iOverlapCount;

public:
    bool IsPause() { return m_bPause; }
    void SetPause() { m_bPause = true; }
    void ReleasePause() { m_bPause = false; }
public:
    void BeginOverlap(CCollider* _pOther);
    void Overlap(CCollider* _pOther);
    void EndOverlap(CCollider* _pOther);

public:
    float GetRotationX() { return m_vRot.x; }
    float GetRotationY() { return m_vRot.y; }
    float GetRotationZ() { return m_vRot.z; }
    void  SetRotation(Vec3 vRot);

    Vec3 GetRotation() { return m_vRot; }
    const Matrix& GetWorldMat() { return m_matWorld; }

    void SetIgnoreObjectScale(bool _bSet) { m_bIgnorObjectScale = _bSet; }
    bool GetIgnoreObjectScale() { return m_bIgnorObjectScale; }
    int GetOverlapCount() { return m_iOverlapCount; }
    void ResetOverlapCount() { m_iOverlapCount = 0; }
public:
    virtual void finaltick() override;
public:
    virtual void SaveToFile(FILE* _File);
    virtual void LoadFromFile(FILE* _File);
    CLONE(CCollider);
public:
    CCollider(COMPONENT_TYPE _type);
    virtual ~CCollider();
};

