#ifdef TESTING

#include <gtest/gtest.h>

#include "EventedSystem.h"
#include "Entity.h"
#include "Event.h"
#include <vector>

using namespace Tactics;
using namespace Tactics::ECS;

struct NameComponent : Component {
	std::string name;
};

struct IntComponent : Component {
	int i = 0;
};

struct SystemTestEvent : public ECS::Event {};

/*
  This class is a model of how the DrawingSystem will be constructed, by inheriting from EventedSystem and IncludesFilteredSystem
*/
// TODO suppress dominance warning... the dominance is exactly what we want
struct DummyFilteredEventedSystem : public virtual EventedSystem<SystemTestEvent>, public virtual ECS::IncludesFilteredSystem {
	DummyFilteredEventedSystem() : IncludesFilteredSystem(MaskBuilder().set<NameComponent>().mask()) {};
	void run(std::vector<Entity> & hdls) {
		size = hdls.size();
	}
	int size = 0;
};


TEST(Systems, filtered_evented_system) {
	ECS::World world;
	
	// Set up 2 entities with different components
	EntityHdl e1 = world.newEntity();
	NameComponent * nc = world.addComponent<NameComponent>(e1);
	nc->name = "win";
	
	EntityHdl e2 = world.newEntity();
	IntComponent * ic = world.addComponent<IntComponent>(e2);
	ic->i = 42;

	DummyFilteredEventedSystem dfes;
	world.registerSystem(dfes);

	// fire an event to trigger the system
	EventDispatcher::postEvent(SystemTestEvent());

	ASSERT_EQ(1, dfes.size) << "There should be 1 entity that matches the filter mask";
}


#endif
