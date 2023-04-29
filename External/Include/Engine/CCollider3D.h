#pragma once
#include "CCollider.h"

static char CCollider3D_Key[100] = "CCollider3D";
class CCollider3D
    :public CCollider
    , public SmallObjAllocator<CCollider3D, OBJECTPOOL_SIZE, CCollider3D_Key>
{
private:
    bool			m_bUpdate = true;
    UINT            m_iBoneIdx = 0;
    Vec3            m_vOffsetPos;
    Vec3            m_vRotOffset{};
    Vec3            m_vScale;

    Vec3            m_vFinalPos;
    Vec3            m_vFinalScale;

    COLLIDER3D_TYPE m_eType;
public:
    void SetOffsetPos(Vec3 _vOffset) { m_vOffsetPos = _vOffset; }
    void SetRotOffset(Vec3 _vOffset) { m_vRotOffset = _vOffset; }
    Vec3 GetRotOffset() { return m_vRotOffset; }
    void SetScale(Vec3 _vScale) { m_vScale = _vScale; }

    void SetCollider3DType(COLLIDER3D_TYPE _type) { m_eType = _type; }
    COLLIDER3D_TYPE GetCollider3DType() { return m_eType; }

    Vec3 GetOffsetPos() { return m_vOffsetPos; }
    Vec3 GetScale() { return m_vScale; }

    Vec3 GetFinalPos() { return m_vFinalPos; }

    void SetIsUpdate(bool _bUpdate) { m_bUpdate = _bUpdate; }
    bool GetIsUpdate() { return m_bUpdate; }

    int GetBoneIdx() { return m_iBoneIdx; }
    void SetBoneIdx(int idx) { m_iBoneIdx = idx; }
public:
    virtual void finaltick() override;
public:
    virtual void SaveToFile(FILE* _File);
    virtual void LoadFromFile(FILE* _File);
    CLONE(CCollider3D);
public:
    CCollider3D();
    virtual ~CCollider3D();
};

