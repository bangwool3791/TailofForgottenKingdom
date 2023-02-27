#include "pch.h"
#include "pfgen.h"

#include "CTimeMgr.h"

using namespace cyclone;
void ParticleForceRegistry::updateForces(real duration)
{
	Registry::iterator i = registrations.begin();
	for (; i != registrations.end(); i++)
	{
		i->fg->updateForce(i->particle, duration);
	}
}


ParticleGravity::ParticleGravity(const PxVec3& gravity)
	:gravity{gravity}
{
}

void ParticleGravity::updateForce(Particle* particle, real duration)
{
	// Check that we do not have infinite mass.
	if (!particle->hasFiniteMass()) return;
	// Apply the mass-scaled force to the particle.
	particle->addForce(gravity * particle->getMass());
}