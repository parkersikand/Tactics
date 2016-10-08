#ifdef TESTING

#include "Entity.h"
using namespace Tactics::ECS;

#include <gtest/gtest.h>

#include <algorithm>

TEST(World, Create_world) {
	World world;
	ASSERT_TRUE(true) << "We should not have died";
}

struct NameComponent : Component {
	std::string name = "";
};

TEST(World, Basic_entity_component_1) {
	World world;
	EntityHdl hdl = world.newEntity();
	ASSERT_EQ(1, hdl) << "First entity should get first id";
	world.addComponent<NameComponent>(hdl);
	NameComponent * ncptr = world.getComponent<NameComponent>(hdl);
	ASSERT_EQ(std::string(), ncptr->name);
}

TEST(World, Basic_entity_component_2) {
	World world;
	EntityHdl hdl = world.newEntity();
	ASSERT_EQ(1, hdl) << "First entity should get first id";
	world.addComponent<NameComponent>(hdl);
	NameComponent * ncptr = world.getComponent<NameComponent>(hdl);
	ASSERT_EQ(std::string(), ncptr->name);
	ncptr->name = "MyName";
	NameComponent * ncptr2 = world.getComponent<NameComponent>(hdl);
	ASSERT_EQ("MyName", ncptr2->name);
}

struct PositionComponent : Component {
	double x;
	double y;
};

TEST(World, filter_mask_1) {
	World world;
	EntityHdl eWithName = world.newEntity();
	NameComponent nc;
	nc.name = "myname";
	world.addComponent<NameComponent>(eWithName, nc);
	EntityHdl eWithPosition = world.newEntity();
	PositionComponent pc;
	pc.x = 1;
	pc.y = 2;
	world.addComponent<PositionComponent>(eWithPosition, pc);
	ComponentMask mask;
	mask.set<NameComponent>();
	auto esWithName = world.filterMaskExact(mask);
	ASSERT_EQ(1, esWithName.size());
	ASSERT_EQ(eWithName, esWithName[0]);
	ASSERT_EQ(std::string("myname"), world.getComponent<NameComponent>(eWithName)->name);
}

TEST(World, filter_mask_includes) {
	World world;
	EntityHdl eWithName = world.newEntity();
	NameComponent nc;
	nc.name = "myname";
	world.addComponent<NameComponent>(eWithName, nc);
	EntityHdl eWithPositionAndName = world.newEntity();
	NameComponent nc2;
	nc2.name = "myname2";
	world.addComponent<NameComponent>(eWithPositionAndName, nc2);
	PositionComponent pc;
	pc.x = 1;
	pc.y = 2;
	world.addComponent<PositionComponent>(eWithPositionAndName, pc);
	ComponentMask mask;
	mask.set<NameComponent>();
	auto esWithName = world.filterMaskIncludes(mask);
	ASSERT_EQ(2, esWithName.size());

	std::vector<std::string> enames;
	enames.push_back(world.getComponent<NameComponent>(esWithName[0])->name);
	enames.push_back(world.getComponent<NameComponent>(esWithName[1])->name);
	sort(enames.begin(), enames.end());

	std::vector<std::string> check = { "myname", "myname2" };
	sort(check.begin(), check.end());

	ASSERT_EQ(check, enames);
}

#endif
