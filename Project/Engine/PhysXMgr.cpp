#include "pch.h"
#include "timing.h"
#include "PhysXMgr.h"
#include "PhysXComponent.h"
#include "CTransform.h"


// Method definitions
PhysXMgr::PhysXMgr()
    : currentPhysType(ARTILLERY)
{
    // Make all shots unused
    for (cyclone::AmmoRound* shot = ammo; shot < ammo + AmmoRounds; shot++)
    {
        shot->type = UNUSED;
    }
}

PhysXMgr::~PhysXMgr()
{
    TimingData::deinit();
}

void PhysXMgr::init()
{
    TimingData::init();
    m_pMesh = CResMgr::GetInst()->FindRes<CMesh>(L"AreneStage_Navi_Mesh");
    m_vecTransform.resize(0xffff);
}

void PhysXMgr::add(CPhysXComponent* pCom)
{
    assert(pCom->Transform());

    float fSpeed = 3.5f;

    cyclone::AmmoRound* shot;

    Vec3 vFront = pCom->Transform()->GetRelativeDir(DIR::FRONT);

    UINT idx = 0;
    // Find the first available round.

    for (shot = ammo; shot < ammo + AmmoRounds; shot++)
    {
        if (shot->type == UNUSED)
        {
            m_vecTransform[idx] = pCom->Transform();
            shot->idx = idx++;
            break;
        }
    }


    // If we didn't find a round, then exit - we can't fire.
    if (shot >= ammo + AmmoRounds) return;

    pCom->SetType(currentPhysType);
    // Set the properties of the particle
    switch (currentPhysType)
    {
    case PISTOL:
        shot->particle.setMass(2.0f); // 2.0kg
        shot->particle.setVelocity(0.0f, 0.0f, 35.0f); // 35m/s
        shot->particle.setAcceleration(0.0f, -1.0f, 0.0f);
        shot->particle.setDamping(0.99f);
        break;

    case ARTILLERY:
        shot->particle.setMass(5.0f); // 200.0kg
        shot->particle.setVelocity(vFront.x * fSpeed, fSpeed + 2.f, vFront.z * fSpeed); // 50m/s
        shot->particle.setAcceleration(0.0f, -9.8f, 0.0f);
        shot->particle.setDamping(0.99f);
        break;

    case FIREBALL:
        shot->particle.setMass(1.0f); // 1.0kg - mostly blast damage
        shot->particle.setVelocity(0.0f, 0.0f, 10.0f); // 5m/s
        shot->particle.setAcceleration(0.0f, 0.6f, 0.0f); // Floats up
        shot->particle.setDamping(0.9f);
        break;

    case LASER:
        // Note that this is the kind of laser bolt seen in films,
        // not a realistic laser beam!
        shot->particle.setMass(0.1f); // 0.1kg - almost no weight
        shot->particle.setVelocity(0.0f, 0.0f, 100.0f); // 100m/s
        shot->particle.setAcceleration(0.0f, 0.0f, 0.0f); // No gravity
        shot->particle.setDamping(0.99f);
        break;
    }

    // Set the data common to all particle types
    shot->particle.setPosition(0.f, 0.f, 0.f);
    shot->startTime = TimingData::get().lastFrameTimestamp;
    shot->type = currentPhysType;

    // Clear the force accumulators
    shot->particle.clearAccumulator();

    pCom->Set_AmmoRound(shot);
    pCom->Run(true);
}

void PhysXMgr::update()
{
    // Find the duration of the last frame in seconds
    float duration = (float)TimingData::get().lastFrameDuration * 0.001f;
    if (duration <= 0.0f) return;

    // Update the physics of each particle in turn
    for (cyclone::AmmoRound* shot = ammo; shot < ammo + AmmoRounds; shot++)
    {
        if (shot->type != UNUSED && shot->type != PAUSE)
        {
            // Run the physics
            shot->particle.integrate(duration);

            assert(0xffff != shot->idx);
            // Check if the particle is now invalid
            Vec3 vPos = m_vecTransform[shot->idx]->GetRelativePos();

            if (m_pMesh->IsNavJumpValid(vPos) &&
                shot->startTime + 150.f < TimingData::get().lastFrameTimestamp)
                // shot->startTime + 5000 < TimingData::get().lastFrameTimestamp ||
                // shot->particle.getPosition().z > 500.0f)
            {
                // We simply set the shot type to be unused, so the
                // memory it occupies can be reused by another shot.
                shot->type = UNUSED;
                m_vecTransform[shot->idx]->SetRelativePos(vPos);
            }

            if (shot->type == UNUSED && shot->idx != 0xffff)
            {
                m_vecTransform[shot->idx] = nullptr;
                shot->idx = 0xffff;
            }
        }
    }
}