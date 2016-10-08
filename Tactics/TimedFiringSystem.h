#pragma once

#ifndef __TACTICS_TIMED_FIRING_SYSTEM_H__
#define __TACTICS_TIMED_FIRING_SYSTEM_H__

#include "Event.h"
#include "System.h"

#include <GLFW/glfw3.h>

namespace Tactics {

	namespace Events {
		struct TimerEvent : ECS::Event {
			double now;
			double last;
			TimerEvent(double n, double l) : now(n), last(l) {};
		};
	} // namespace Events

	template <typename EventType = TimerEvent>
	class TimedFiringSystem : public ECS::RunnableSystem {
	public:
		
		explicit TimedFiringSystem(double ms) : interval(ms) {}

		double getInterval() const { return interval; }

		virtual void run() {
			double timeNow = glfwGetTime() * 1000;
#ifdef TACTICS_VERBOSE_DEBUG
			printf("TimedFiringSystem::run  %f", timeNow);
#endif
			if (timeNow - lastTime > interval) {
				ECS::EventDispatcher::postEvent(createEvent());
				lastTime = timeNow;
			}
		}

	protected:
		virtual EventType createEvent() {
			return EventType();
		};

		double getLastTime() const { return lastTime; }

	private:
		double interval;
		double lastTime = 0;
	};

} // naespace Tactics


#endif
