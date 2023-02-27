#pragma once
#include <vector>
#include "particle.h"
#include "CComponent.h"
namespace cyclone {
	class ParticleForceGenerator
	{
	public:
		/**
		* Overload this in implementations of the interface to calculate
		* and update the force applied to the given particle.
		*/
		virtual void updateForce(Particle* particle, real duration) = 0;
	};
}

namespace cyclone {
	/**
	* Holds all the force generators and the particles they apply to.
	*/
	class ParticleForceRegistry
		:public CComponent
	{
	protected:
		/**
		* Keeps track of one force generator and the particle it
		* applies to.
		*/
		struct ParticleForceRegistration
		{
			Particle* particle;
			ParticleForceGenerator* fg;
		};
		/**
		* Holds the list of registrations.
		*/
		typedef std::vector<ParticleForceRegistration> Registry;
		Registry registrations;
	public:
		/**
		* Registers the given force generator to apply to the
		* given particle.
		*/
		void add(Particle* particle, ParticleForceGenerator* fg);
		/**
		* Removes the given registered pair from the registry.
		* If the pair is not registered, this method will have
		* no effect.
		*/
		void remove(Particle* particle, ParticleForceGenerator* fg);
		/**
		* Clears all registrations from the registry. This will
		* not delete the particles or the force generators
		* themselves, just the records of their connection.
		*/
		void clear();
		/**
		* Calls all the force generators to update the forces of
		* their corresponding particles.
		*/
		void updateForces(real duration);
	};


	class ParticleGravity : public ParticleForceGenerator
	{
		/** Holds the acceleration due to gravity. */
		PxVec3 gravity;
	public:
		/** Creates the generator with the given acceleration. */
		ParticleGravity(const PxVec3& gravity);
		/** Applies the gravitational force to the given particle. */
		virtual void updateForce(Particle* particle, real duration);
	};
}