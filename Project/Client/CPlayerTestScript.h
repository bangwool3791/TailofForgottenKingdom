#pragma once
#include <Engine\CScript.h>

class CGameObjectEx;
class CCollider;
class CMesh;

class CPlayerTestScript :
    public CScript
{
private:
    bool                    m_bDash[4];
    bool                    m_bRoll[2];
    bool                    m_bRollAttack;
    bool                    m_bAirDash;
    bool                    m_bAttack;
    bool                    m_bSwordTrail = false;
    bool                    m_bJump;
    UINT                    m_iLButtonCount = 0;
    float                   m_fAccTime;
    float                   m_fSpeed;
    float                   m_fJumpHeight;
    float                   m_fAttackDelay = 1.f;
    float                   m_fSworTrailSpeed = 1.f;
    CAM_DIR                 m_eDir = CAM_DIR::FRONT;
    Vec3                    m_vPos;
    Ptr<CPrefab>            m_Prefab;
    Ptr<CMesh>              m_pTriangleMesh;
    vector<CAnimator3D*>    m_vecAnimation;

    std::vector<float>      m_vecTrailTime;
    std::vector<float>      m_vecSwordTrailTime;

    CGameObjectEx*          m_pBoneSocketObj;
    CGameObjectEx*          m_pUpperSwordObj;
    CGameObjectEx*          m_pBottomSwordObj;
    CGameObjectEx*          m_pSworTrailObj;
    CGameObjectEx*          m_pBloodParticle;
    vector<CGameObjectEx*>   m_vecObjTrail;
    vector<CGameObjectEx*>   m_vecSwordTrail;
    
    void Attack();
    void Dir();
    void Jump();
    void Etc();
    void Set_Animation_Key(const wstring& _strKey);
    void Set_Animation_Time(float);
    void Move();
    void TrailProcedure();
    bool ClearAnimation();
    void PlayerTrail();

    bool IsDash();
    void ClearDash();

    UINT GetDashDir();

    bool IsRoll();
    void ClearRoll();

    UINT GetRollDir();
public:
    CAM_DIR GetDir() { return m_eDir; }
private:
    void SaveToFile(FILE* _File);
    void LoadFromFile(FILE* _File);
public:
    virtual void begin();
    virtual void tick();
    virtual void finaltick();
    virtual void BeginOverlap(CCollider* _pOther);
    virtual void Overlap(CCollider* _pOther);
    virtual void EndOverlap(CCollider* _pOther);
    CLONE(CPlayerTestScript);
public:
    CPlayerTestScript();
    virtual ~CPlayerTestScript();
};

