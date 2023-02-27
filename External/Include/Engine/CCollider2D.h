#pragma once
#include "CCollider.h"

static char CCollider2D_Key[100] = "CCollider2D";
class CCollider2D
    :public CCollider
    , public SmallObjAllocator<CCollider2D, OBJECTPOOL_SIZE, CCollider2D_Key>
{
private:
    Vec2            m_vOffsetPos;
    Vec2            m_vScale;

    Vec2            m_vFinalPos;
    Vec2            m_vFinalScale;

    COLLIDER2D_TYPE m_eType;
public:
    void SetOffsetPos(Vec2 _vOffset) { m_vOffsetPos = _vOffset; }
    void SetScale(Vec2 _vScale) { m_vScale = _vScale; }

    void SetCollider2DType(COLLIDER2D_TYPE _type) { m_eType = _type; }
    COLLIDER2D_TYPE GetCollider2DType() { return m_eType; }

    Vec2 GetOffsetPos() { return m_vOffsetPos; }
    Vec2 GetScale() { return m_vScale; }

    Vec2 GetFinalPos() { return m_vFinalPos; }
public:
    virtual void finaltick() override;

public:
    virtual void SaveToFile(FILE* _File);
    virtual void LoadFromFile(FILE* _File);
    CLONE(CCollider2D);
public:
    CCollider2D();
    virtual ~CCollider2D();
};

