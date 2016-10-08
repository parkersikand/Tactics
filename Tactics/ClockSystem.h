#pragma once


#ifndef __TACTICS_CLOCKSYSTEM_H__
#define __TACTICS_CLOCKSYSTEM_H__

#include "System.h"
#include "Event.h"

namespace Tactics {


	struct ClockEvent : public ECS::Event {
		double delta;
	};

    
	// fires a ClockEvent every specified time period
	// meant to be used with RunnableWorld
	class ClockSystem : public ECS::System {
	public:
		ClockSystem() {};
		ClockSystem(double cs) : fireSpeedMS(cs) {};
		virtual void run();
	private:
		double fireSpeedMS = 30;
		double lastTime;
	};


} // namespace Tactics














#endif
