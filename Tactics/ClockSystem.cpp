


#include "ClockSystem.h"

#include <GLFW/glfw3.h>


using namespace Tactics;
using namespace Tactics::ECS;


void ClockSystem::run() {
	static ClockEvent ce; // optimization to avoid creating a new event every tick
	double curTime = glfwGetTime();
	if (curTime - lastTime < fireSpeedMS) return;
	ce.delta = curTime - lastTime;
	EventDispatcher::postEvent<ClockEvent>(ce);
	lastTime = curTime;
}


