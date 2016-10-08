#ifdef TESTING

#include <gtest/gtest.h>


#include "Component.h"

using namespace Tactics::ECS;

// test components

struct PositionComponent {
	double x;
	double y;
};


struct NameComponent {
	std::string name;
};


TEST(Components, component_manager_id_handout) {
	ComponentId posId = ComponentManager::getComponentId<PositionComponent>();
	ASSERT_EQ(1, posId) << "The first component gets id 1";
	ComponentId nameId = ComponentManager::getComponentId<NameComponent>();
	ASSERT_EQ(2, nameId) << "The second component gets id 2";
	ComponentId posId2 = ComponentManager::getComponentId<PositionComponent>();
	ASSERT_EQ(1, posId2) << "The first id is now set and does not change";
}



#endif
