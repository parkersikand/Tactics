
#include "Entity.h"
#include "System.h"


using namespace Tactics::ECS;


// kill all humans
World::~World() {
	for (unsigned int c = 0; c < ComponentManager::getSize(); c++) {
		for (unsigned int i = 0; i < current_size; i++) {
			if (container[c][i] != nullptr) delete container[c][i];
		}
		delete container[c];
	}
	delete mask_arr;

	for (unsigned int i = 0; i < managedSystems.size(); i++) {
		if (managedSystems[i] != nullptr) delete managedSystems[i];
	}
}


World::World() {
	mask_arr = new ComponentMask[current_size];
	for (unsigned i = 0; i < MAX_COMPONENTS; i++) {
		container[i] = new Component *[current_size];
		for (unsigned cs = 0; cs < current_size; cs++) {
			container[i][cs] = nullptr;
		}
	}
}


void World::runAllSystems() {
	// run all registered systems
	for (auto sp : systems) {
		sp->run(*this);
	}
}


void World::registerSystem(System & system) {
	system.world = this;
	system.onRegister(*this);
}


void World::addSystem(System & system) {
	registerSystem(system);
	systems.push_back(&system);
}


EntityHdl World::addEntity(Entity & entity) {
	EntityHdl hdl = idx;
	idx++;
	mask_arr[hdl] = entity.getMask();
	return hdl;
}


void RunnableWorld::addRunnableSystem(RunnableSystem & rs) {
	registerSystem(rs);
	systems.push_back(&rs);
}


void RunnableWorld::run() {
	while (true) {
		for (unsigned i = 0; i < systems.size(); i++) {
			if (systems[i]->isRunning()) {
				systems[i]->run(*this);
				
				// if the system has shut down the world, return from the run loop function
				if (!running) {
					return;
				}
			}
		}
	}
}


void RunnableWorld::start() {
	setup();
	running = true;
	for (auto & system : systems) {
		system->start();
	}
	run();
	for (auto & system : systems) {
		system->stop();
	}
	shutdown();
}


void RunnableWorld::stop() {
	running = false;
}


void RunnableWorld::registerRunnableSystem(RunnableSystem & system) {
	World::registerSystem(system);
	system.onRegister(*this);
}
