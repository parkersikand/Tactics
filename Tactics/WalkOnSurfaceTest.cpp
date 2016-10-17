#ifdef TESTING


#include "WalkOnSurface.h"
#include "DrawLoop.h"
#include "Object3D.h"
#include "CameraSystem.h"

#include <gtest/gtest.h>

using namespace Tactics;

TEST(WalkOnSurface, visualize_level_1) {
	Tactics::Worlds::WalkOnSurfaceWorld world;
	world.setup();

	// add custom camera
	ECS::EntityHdl camera = world.newEntity();
	auto * cameraPos = world.addComponent<Components::Position3D<>>(camera);
	cameraPos->y = 5.f;
	cameraPos->x = 5.f;
	cameraPos->z = 5.f;
	auto * cameraComp = world.addComponent<Components::CameraComponent>(camera);
	cameraComp->lookAt = glm::vec3(0.f, 0.f, 0.f);
	Events::CameraChangedEvent cce;
	cce.newCamera = camera;
	ECS::EventDispatcher::postEvent(cce);

	// issue draw call
	Tactics::DrawEvent de;
	Tactics::ECS::EventDispatcher::postEvent(de);
	std::cout << "Press enter to continue..." << std::endl;
	getchar();
	SUCCEED();
}


TEST(WalkOnSurface, Step1) {
	Tactics::Worlds::WalkOnSurfaceWorld world;
	world.setup();
	ASSERT_TRUE(world.testStep1());
}




#endif
