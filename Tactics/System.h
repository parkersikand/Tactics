#pragma once

#ifndef __ECS_SYSTEM_H__
#define __ECS_SYSTEM_H__

#include "Entity.h"
#include "Component.h"
#include "Event.h"

#include "Constants.h"

namespace Tactics {

	namespace ECS {

		// System is an abstract base class for concrete systems
		class System {
		public:
			// virtual destructor
			virtual ~System() {
			    // unregister the world and let everything else get deleted
				world = nullptr;
			}

			// by default, call run discarding the world
			virtual void run(World &) { run(); };

			// by default, do nothing
			virtual void run() {};

			// method to destroy the system
			virtual void shutdown() {};

			// method called upon registration
			virtual void onRegister(World &) {};

		protected:
			World * getWorld() { return world; }
		private:
			World * world;
			friend class World;
		};


		// A system meant to be used with a RunnableWorld
		// Overrides and provieds RunnableWorld to run method
		class RunnableSystem : public virtual System {
		public:
			virtual void run(RunnableWorld &) {
				run();
			};
			virtual void run() {};
			const bool isRunning() const { return running; }
			virtual void start() { running = true; }
			virtual void stop() { running = false; }

			virtual void onRegister(RunnableWorld &) {};

		private:
			bool running = true;
		};


		class CachedSystem : virtual System {
		public:
		protected:
			virtual void buildCache(World) = 0;
			virtual void run(World &) = 0;
		private:
			std::vector<EntityHdl> cache;
		};


		class IncludesFilteredSystem : public virtual System {
		public:
			IncludesFilteredSystem(ComponentMask m) : mask(m) {};

			virtual void run(World & w) final {
				run(w.entitiesFromHandles(w.filterMaskIncludes(mask)));
			};

			virtual void run(std::vector<Entity> & entities) = 0;
		
		protected:
			// fetch the matching entities from the world
			std::vector<EntityHdl> getEntities() {
				return getWorld()->filterMaskIncludes(mask);
			}

		private:
			ComponentMask mask;
		};
		

	} // namespace ECS

} // namespace Tactics


#endif
