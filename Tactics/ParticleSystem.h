#pragma once

#ifndef __TACTICS_PARTICLE_SYSTEM_H__
#define __TACTICS_PARTICLE_SYSTEM_H__

#include "Component.h"
#include "System.h"
#include "Entity.h"
#include "Event.h"
#include "DrawSystem.h"
#include "EventedSystem.h"
//#include "DrawLoop.h"
#include "DrawSystem.h"

#include <GL/glew.h>
#include "glm.hpp"

namespace Tactics {

	// forward declarations
	namespace Systems {
		class ParticleSystem;
	}

	namespace Components {

		struct Particle : public ECS::Component {
			Systems::ParticleSystem * particleSystem;
			glm::vec4 color;
			double birthTime;
		};

	}

	namespace Systems {

		class ParticleSystem : public ECS::RunnableSystem {
		public:
			ParticleSystem(unsigned int mp) : maxParticles(mp) {};

			// create a new particle
			ECS::EntityHdl emitParticle();

			unsigned int numParticles() { return particles.size(); }
			unsigned int getMaxParticles() { return maxParticles; }

			// override to define how the particles update
			virtual void run(ECS::World &, std::vector<ECS::EntityHdl> &, double) = 0;

		private:
			unsigned int maxParticles;

			std::vector<ECS::EntityHdl> particles;

			// time we last updated
			double lastTime;
		};

		
		class ParticleDrawSystem : public AbstractDrawSystem<ParticleDrawSystem> {
		public:
			
			void run(std::vector<ECS::Entity> &);

			void setProgramId(GLuint p) { programId = p; }
		
		private:
			GLuint programId;
		};
		
	}

}

#endif
