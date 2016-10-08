#pragma once

#ifndef __ECS_THREADED_WORLD
#define __ECS_THREADED_WORLD

#include "Entity.h"
#include "Event.h"

#include <thread>

namespace Tactics {

	namespace ECS {

		// A runnable world that runs in its own thread
		class ThreadedWorld : public ECS::RunnableWorld {
		public:

			// override the start function
			void start();

		private:
			std::thread thread;
		}; // ThreadedWorld

	} // namespace Tactics::ECS 

} // namespace Tactics



#endif
