#pragma once
#include "pch.h"
#include "CComponent.h"

class CPhysXComponent
    :public CComponent
{
private:
    bool m_bRun = false;
    PhysType m_ePhysType;
    cyclone::AmmoRound* AmmoRound = nullptr;
public:
    void Set_AmmoRound(cyclone::AmmoRound* pRound) { AmmoRound = pRound; }
    cyclone::AmmoRound* Get_AmmoRound() { return AmmoRound; }
    void Run(bool _bRun) { m_bRun = _bRun; }
    bool IsRelease();
    bool GetRun() { return m_bRun; }
    void SetType(PhysType eType) { m_ePhysType = eType; }
    PhysType GetType() { return m_ePhysType; }
public:
    virtual void begin();
    virtual void tick() {};
    virtual void finaltick();
public:
    virtual void SaveToFile(FILE* _File);
    virtual void LoadFromFile(FILE* _File);
    CLONE(CPhysXComponent);
public:
    CPhysXComponent();
    CPhysXComponent(const CPhysXComponent& _rhs);
    virtual ~CPhysXComponent();
};

