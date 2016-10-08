
#include "ThreadedSystem.h"


using namespace Tactics;
using namespace ECS;


void ThreadedSystem::start() {
	running = true;
	thread = std::thread(&ThreadedSystem::threadedRun, this);
}

void ThreadedSystem::threadedRun() {
	while (running) {
		run();
	}
}

void ThreadedSystem::stop() {
	running = false;
	RunnableSystem::stop();
}

