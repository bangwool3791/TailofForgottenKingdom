#pragma once
#include <Engine\CScript.h>

class CCollider;
class CMesh;

class CPlayerScript :
    public CScript
{
private:
    float                   m_fAccTime;
    float                   m_fSpeed;
    float                   m_fJumpHeight;

    Vec3                    m_vPos;
    Ptr<CPrefab>            m_Prefab;
    Ptr<CMesh>              m_pTriangleMesh;
    vector<CAnimator3D*>    m_vecAnimation;

    void Set_Animation_Key(const wstring& _strKey);
private:
    void SaveToFile(FILE* _File);
    void LoadFromFile(FILE* _File);
public :
    virtual void begin();
    virtual void tick();
    virtual void BeginOverlap(CCollider* _pOther);
    virtual void Overlap(CCollider* _pOther);
    virtual void EndOverlap(CCollider* _pOther);
    CLONE(CPlayerScript);
public:
    CPlayerScript();
    virtual ~CPlayerScript();
};

