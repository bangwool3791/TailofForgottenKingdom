#pragma once
#include <Engine\CScript.h>

class CCollider;
class CMesh;

class CPlayerScript :
    public CScript
{
private:
    bool                    m_bAttack;
    bool                    m_bJump;
    UINT                    m_iLButtonCount = 0;
    float                   m_fAccTime;
    float                   m_fSpeed;
    float                   m_fJumpHeight;

    Vec3                    m_vPos;
    Ptr<CPrefab>            m_Prefab;
    Ptr<CMesh>              m_pTriangleMesh;
    vector<CAnimator3D*>    m_vecAnimation;

    void Set_Animation_Key(const wstring& _strKey);
    void Set_Animation_Time(float);
    void Move();
    bool ClearAnimation();
    void Test();
private:
    void SaveToFile(FILE* _File);
    void LoadFromFile(FILE* _File);
public :
    virtual void begin();
    virtual void tick();
    virtual void finaltick();
    virtual void BeginOverlap(CCollider* _pOther);
    virtual void Overlap(CCollider* _pOther);
    virtual void EndOverlap(CCollider* _pOther);
    CLONE(CPlayerScript);
public:
    CPlayerScript();
    virtual ~CPlayerScript();
};

