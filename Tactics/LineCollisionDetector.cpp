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
	EntityHdl sourceHdl, cubeHdl, cube2Hdl;

	virtual void setup() {
		Tactics::Worlds::BasicWorld::setup();

		// ray source
		sourceHdl = newEntity();
		auto * pos = addComponent<Components::Position3D<>>(sourceHdl);
		pos->y = 0.5;
		addComponent<LineCollision::Components::LineCollisionSource>(sourceHdl);
		auto * ray = addComponent<LineCollision::Components::LineCollisionRay>(sourceHdl);
		ray->direction = glm::vec3(1.f, 0.f, 0.f);
		addComponent<Components::ModelTransform>(sourceHdl);

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
};

TEST(LineCollision, simple_line_collision_hit) {
	SimpleLineCollision1World world;
	world.setup();
	ASSERT_TRUE(world.testHit());
}

TEST(LineCollisoin, simple_line_collision_miss) {
	SimpleLineCollision1World world;
	world.setup();
	ASSERT_FALSE(world.testMiss());
}

#endif