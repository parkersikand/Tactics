#pragma once

#ifndef __ECS_THREADED_SYSTEM_H__
#define __ECS_THREADED_SYSTEM_H__

#include "System.h"

#include <thread>

namespace Tactics {

	namespace ECS {

		// A system that runs in its own thread
		class ThreadedSystem : public virtual RunnableSystem {
		public:

			virtual void start();
			virtual void stop();

		protected:
			bool running = false;
			// This is the loop
			virtual void threadedRun();
			std::thread thread;

		};

		// Turn a runnable system into a threaded system
//		template <typename RunnableSystemType>
//		class ThreadedSystemAdapter : public virtual ThreadedSystem, public virtual RunnableSystemType {};

		
		template <typename RunnableSystemType>
	    class ThreadedSystemAdapter : public virtual ThreadedSystem {
		public:
			~ThreadedSystemAdapter() { delete ptr; }
			ThreadedSystemAdapter() : ptr(new RunnableSystemType) {};
			ThreadedSystemAdapter(RunnableSystemType * p) : ptr(p) {};

		private:
			void threadedRun() {
				while (running) {
					ptr->run();
				}
			}
			RunnableSystemType * ptr;
		};
		

	}

}


#endif
