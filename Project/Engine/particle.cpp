/*
 * Implementation file for the particle class.
 *
 * Part of the Cyclone physics system.
 *
 * Copyright (c) Icosagon 2003. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */
#include "pch.h"
#include "particle.h"

/*
 * --------------------------------------------------------------------------
 * FUNCTIONS DECLARED IN HEADER:
 * --------------------------------------------------------------------------
 */

using namespace cyclone;

void Particle::integrate(PhysXReal duration)
{
    // We don't integrate things with zero mass.
    if (inverseMass <= 0.0f) return;

    assert(duration > 0.0);

    // Update linear position.
    position.addScaledVector(velocity, duration);

    // Work out the acceleration from the force
    PhysXVec3 resultingAcc = acceleration;
    resultingAcc.addScaledVector(forceAccum, inverseMass);

    // Update linear velocity from the acceleration.
    velocity.addScaledVector(resultingAcc, duration);

    // Impose drag.
    velocity *= real_pow(damping, duration);

    // Clear the forces.
    clearAccumulator();
}



void Particle::setMass(const PhysXReal mass)
{
    assert(mass != 0);
    Particle::inverseMass = ((PhysXReal)1.0) / mass;
}

PhysXReal Particle::getMass() const
{
    if (inverseMass == 0) {
        return REAL_MAX;
    }
    else {
        return ((PhysXReal)1.0) / inverseMass;
    }
}

void Particle::setInverseMass(const PhysXReal inverseMass)
{
    Particle::inverseMass = inverseMass;
}

PhysXReal Particle::getInverseMass() const
{
    return inverseMass;
}

bool Particle::hasFiniteMass() const
{
    return inverseMass >= 0.0f;
}

void Particle::setDamping(const PhysXReal damping)
{
    Particle::damping = damping;
}

PhysXReal Particle::getDamping() const
{
    return damping;
}

void Particle::setPosition(const PhysXVec3& position)
{
    Particle::position = position;
}

void Particle::setPosition(const PhysXReal x, const PhysXReal y, const PhysXReal z)
{
    position.x = x;
    position.y = y;
    position.z = z;
}

void Particle::getPosition(PhysXVec3* position) const
{
    *position = Particle::position;
}

PhysXVec3 Particle::getPosition() const
{
    return position;
}

void Particle::setVelocity(const PhysXVec3& velocity)
{
    Particle::velocity = velocity;
}

void Particle::setVelocity(const PhysXReal x, const PhysXReal y, const PhysXReal z)
{
    velocity.x = x;
    velocity.y = y;
    velocity.z = z;
}

void Particle::getVelocity(PhysXVec3* velocity) const
{
    *velocity = Particle::velocity;
}

PhysXVec3 Particle::getVelocity() const
{
    return velocity;
}

void Particle::setAcceleration(const PhysXVec3& acceleration)
{
    Particle::acceleration = acceleration;
}

void Particle::setAcceleration(const PhysXReal x, const PhysXReal y, const PhysXReal z)
{
    acceleration.x = x;
    acceleration.y = y;
    acceleration.z = z;
}

void Particle::getAcceleration(PhysXVec3* acceleration) const
{
    *acceleration = Particle::acceleration;
}

PhysXVec3 Particle::getAcceleration() const
{
    return acceleration;
}

void Particle::clearAccumulator()
{
    forceAccum.clear();
}

void Particle::addForce(const PhysXVec3& force)
{
    forceAccum += force;
}
