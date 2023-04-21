#pragma once

#include "singleton.h"

class CPhysXComponent;

using namespace cyclone;

class PhysXMgr : public CSingleton<PhysXMgr>
{
    /**
     * Holds the maximum number of  rounds that can be
     * fired.
     */
    const static unsigned AmmoRounds = 0xffff;

    /** Holds the particle data. */
    cyclone::AmmoRound ammo[AmmoRounds];

    /** Holds the current shot type. */
    PhysType currentPhysType;
    UINT m_iIdx;
    Ptr<CMesh> m_pMesh = nullptr;
    vector<class CTransform*> m_vecTransform;
    /** Dispatches a round. */
public:
    void add(CPhysXComponent* pCom);
    void update();

public :
    void init();
private:
    /** Creates a new demo object. */
    PhysXMgr();
    ~PhysXMgr();
    friend class CSingleton<PhysXMgr>;
};