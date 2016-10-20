
#include "Entity.h"
#include "System.h"


using namespace Tactics::ECS;


Entity::Entity(World & w) : world(&w) {
	hdl = world->newEntity();
}


EntityHdl Entity::newChild() {
	EntityHdl child = world->newEntity();
	world->setParentChild(hdl, child);
	return child;
}


void Entity::destroy() {
	world->removeEntity(hdl);
	world = NULL;
	hdl = 0;
	mask = NULL;
	delete this;
}

// World  /////////////////////////////////////////////////////////////////////

unsigned int World::_global_system_counter = 1;

// kill all humans
World::~World() {
	for (unsigned int c = 0; c < ComponentManager::getSize(); c++) {
		for (unsigned int i = 0; i < current_size; i++) {
			if (container[c][i] != nullptr) delete container[c][i];
		}
		delete container[c];
	}

	delete [] mask_arr;

	for (unsigned int i = 0; i < managedSystems.size(); i++) {
		if (managedSystems[i] != nullptr) delete managedSystems[i];
	}
}


World::World() {
	//mask_arr = new ComponentMask[current_size];
	mask_arr = new ComponentMask *[current_size];
	for (unsigned int i = 0; i < current_size; i++) {
		mask_arr[i] = new ComponentMask;
	}
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
	//EntityHdl hdl = idx;
	//idx++;
	EntityHdl hdl = newEntity();
	*mask_arr[hdl] = entity.getMask();
	return hdl;
}


EntityHdl World::newEntity() {
	if (reclaimPool.empty()) {
		EntityHdl hdl = highwater;
		highwater += 1;
		return hdl;
	}
    // clear components and mask ( to be safe)
	EntityHdl hdl = *reclaimPool.begin();
	mask_arr[hdl]->clear();
	for (unsigned int i = 0; i < ECS_MAX_COMPONENTS; i++) {
		if (container[i][hdl]) {  // container[i] should always be valid
			delete container[i][hdl];
		}
		container[i][hdl] = nullptr;
	}
	reclaimPool.erase(reclaimPool.begin());
	return hdl;
}


void World::removeEntity(EntityHdl hdl) {
	// remove children
	for (auto child : entityHierarchy[hdl]) {
		removeEntity(child);
	}

	// delete components
	for (unsigned int i = 0; i < ECS_MAX_COMPONENTS; i++) {
		Component * component = container[i][hdl];
		if (component != nullptr) delete component;
	}

	// unset mask
	mask_arr[hdl]->clear();

	// add entity to reclaim pool
	reclaimPool.insert(hdl);

	current_size -= 1;
	
	// fire entity destroyed event
	ECS::Events::EntityDestroyedEvent ev;
	ev.handle = hdl;
	EventDispatcher::postEvent(ev);
}


void World::setParentChild(EntityHdl parent, EntityHdl child) {
	entityHierarchy[parent].insert(child);
}


EntityHdl World::createChildEntity(EntityHdl parent) {
	EntityHdl child = newEntity();
	setParentChild(parent, child);
	return child;
}


void World::destroyManagedSystem(System * ptr) {
	auto it = std::find(managedSystems.begin(), managedSystems.end(), ptr);
	if (it != managedSystems.end()) {
		managedSystems.erase(it);
	}
	delete ptr;
}


//  RunnableWorld  ////////////////////////////////////////////////////////////

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
