#pragma once

#ifndef __TACTICS_ANIMATION_H__
#define __TACTICS_ANIMATION_H__

#include "Component.h"
#include "System.h"

#include "GLFW/glfw3.h"

namespace Tactics {

	namespace Systems {

		class AnimationSystem : public ECS::RunnableSystem {
		public:

			void run(ECS::RunnableWorld & world) {
				double timeNow = glfwGetTime();
				run(world, entities, timeNow - lastTime);
				lastTime = timeNow;
			}

			// Override this function to perform the animation
			virtual void run(ECS::World &, std::vector<ECS::EntityHdl> &, double) = 0;

			// register an entity to be handled by this system
			void addEntity(ECS::EntityHdl);

			// unregister an entity
			void removeEntity(ECS::EntityHdl);

		private:
			double lastTime;
			std::vector<ECS::EntityHdl> entities;
		};

	}

	namespace Components {

		// the component is just a pointer to the system
		struct Animation : public ECS::Component {
			Systems::AnimationSystem * animationSystem;
		};

	} // namespace Components

} // namespace Tactics


#endif
