
#include "ThreadedWorld.h"

using namespace Tactics;
using namespace Tactics::ECS;

void ThreadedWorld::start() {
	// replace thread object
	thread = std::thread(&RunnableWorld::start, this);
}

