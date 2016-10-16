#ifdef TESTING

#include <gtest/gtest.h>

#include "LineCollision.h"

#include "BasicWorld.h"
#include "Util.h"

using namespace Tactics;
using namespace Tactics::ECS;

//class SimpleLineCollision1World : public Tactics::ECS::RunnableWorld {
class SimpleLineCollision1World : public Tactics::Worlds::BasicWorld {
public:
	EntityHdl sourceHdl, cubeHdl, cube2Hdl, source2;

	virtual void setup() {
		Tactics::Worlds::BasicWorld::setup();

		// ray source
		sourceHdl = newEntity();
		auto * pos = addComponent<Components::Position3D<>>(sourceHdl);
		pos->y = 10.f;
		pos->x = 10.f;
		addComponent<LineCollision::Components::LineCollisionSource>(sourceHdl);
		auto * ray = addComponent<LineCollision::Components::LineCollisionRay>(sourceHdl);
		ray->direction = glm::vec3(-10.f, -10.f, 0.f);
		addComponent<Components::ModelTransform>(sourceHdl);

		// source2
		source2 = newEntity();
		auto * pos2 = addComponent<Components::Position3D<>>(source2);
		pos2->y = 3.f;
		pos2->z = -10.f;
		addComponent<LineCollision::Components::LineCollisionSource>(source2);
		auto * ray2 = addComponent<LineCollision::Components::LineCollisionRay>(source2);
		ray2->direction = glm::vec3(0.f, -3.f, 10.f);
		addComponent<Components::ModelTransform>(source2);

		// cube
		cubeHdl = newEntity();
		auto * cube3d = addComponent<Components::CObject3D>(cubeHdl);
		make_cube(cube3d);
		auto * cubePos = addComponent<Components::Position3D<>>(cubeHdl);
		addComponent<LineCollision::Components::LineCollisionTarget>(cubeHdl);
		addComponent<Components::ModelTransform>(cubeHdl);

		// cube2
		cube2Hdl = newEntity();
		auto * cube2_3d = addComponent<Components::CObject3D>(cube2Hdl);
		make_cube(cube2_3d);
		auto * cube2_Pos = addComponent<Components::Position3D<>>(cube2Hdl);
		cube2_Pos->z = 2.f;
		addComponent<LineCollision::Components::LineCollisionTarget>(cube2Hdl);
		addComponent<Components::ModelTransform>(cube2Hdl);
	}

	// should return true
	bool testHit() {
		LineCollision::Systems::LineCollisionDetector lcd;
		registerSystem(lcd);
		auto * pos = getComponent<Components::Position3D<>>(sourceHdl);
		auto * ray = getComponent<LineCollision::Components::LineCollisionRay>(sourceHdl);
		return lcd.fireRay(*pos, *ray, cubeHdl);
	}

	// should return false
	bool testMiss() {
		LineCollision::Systems::LineCollisionDetector lcd;
		registerSystem(lcd);
		auto * pos = getComponent<Components::Position3D<>>(sourceHdl);
		auto * ray = getComponent<LineCollision::Components::LineCollisionRay>(sourceHdl);
		return lcd.fireRay(*pos, *ray, cube2Hdl);
	}

	bool testBlock() {
		auto * pos = getComponent<Components::Position3D<>>(source2);
		auto * ray = getComponent<LineCollision::Components::LineCollisionRay>(source2);
		auto lcd = *createManagedSystem<LineCollision::Systems::LineCollisionDetector>();
		return lcd.fireRay(*pos, *ray, cube2Hdl);
	}

	bool testResult() {
		auto * pos = getComponent<Components::Position3D<>>(sourceHdl);
		auto * ray = getComponent<LineCollision::Components::LineCollisionRay>(sourceHdl);
		auto lcd = *createManagedSystem<LineCollision::Systems::LineCollisionDetector>();
		auto result = lcd.castResult(source2, cube2Hdl);
		return false;
	}

}; // class SimpleCollision1World

TEST(LineCollision, simple_line_collision_hit) {
	SimpleLineCollision1World world;
	world.setup();
	ASSERT_TRUE(world.testHit());
}

TEST(LineCollision, simple_line_collision_miss) {
	SimpleLineCollision1World world;
	world.setup();
	ASSERT_FALSE(world.testMiss());
}

TEST(LineCollision, line_collision_block) {
	SimpleLineCollision1World world;
	world.setup();
	ASSERT_FALSE(world.testBlock());
}

TEST(LineCollision, line_collision_result) {
	SimpleLineCollision1World world;
	world.setup();
	ASSERT_TRUE(world.testResult());
}

#endif
