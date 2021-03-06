#ifdef TESTING

//#define WALK_ON_SURFACE_TESTS

#ifdef WALK_ON_SURFACE_TESTS

#include "WalkOnSurface.h"
#include "DrawLoop.h"
#include "Object3D.h"
#include "CameraSystem.h"
#include "LineCollision.h"

#include <gtest/gtest.h>

using namespace Tactics;

TEST(WalkOnSurface, visualize_level_1) {
	Tactics::Worlds::WalkOnSurfaceWorld world;
	world.setup();

	// add custom camera
	ECS::EntityHdl camera = world.newEntity();
	auto * cameraPos = world.addComponent<Components::Position3D<>>(camera);
	cameraPos->y = 20.f;
	cameraPos->x = 0.f;
	cameraPos->z = 0.f;
	auto * cameraComp = world.addComponent<Components::CameraComponent>(camera);
	cameraComp->lookAt = glm::vec3(0.f, 0.f, 0.f);
	auto * cameraSystem = world.getGlobalSystem<CameraSystem>();
	cameraSystem->setUp(glm::vec3(-1.f, 0, 0));
	Events::CameraChangedEvent cce;
	cce.newCamera = camera;
	ECS::EventDispatcher::postEvent(cce);
	// add ray component
	auto * ray = world.addComponent<LineCollision::Components::LineCollisionRay>(camera);
	ray->direction = glm::vec3(cameraPos->x, cameraPos->y, cameraPos->z) * (-1.f);
	ray->up = cameraSystem->getUp();

	// call generic cast to generate debug images
	auto lcd = *world.createManagedSystem<LineCollision::Systems::LineCollisionDetector>();
	auto result = lcd.castResult(camera, world.floorHdl);

	// issue draw call
	Tactics::DrawEvent de;
	Tactics::ECS::EventDispatcher::postEvent(de);
	std::cout << "Press enter to continue..." << std::endl;
	getchar();
	SUCCEED();
}

TEST(WalkOnSurface, Visualize_Step_1) {
	Tactics::Worlds::WalkOnSurfaceWorld world;
	world.setup();

	// add custom camera
	ECS::EntityHdl camera = world.newEntity();
	auto * cameraPos = world.addComponent<Components::Position3D<>>(camera);
	cameraPos->y = 2.f;
	cameraPos->x = 0.f;
	cameraPos->z = 0.1f;
	auto * cameraComp = world.addComponent<Components::CameraComponent>(camera);
	cameraComp->lookAt = glm::vec3(0.f, 1.f, 0.1f);
	auto * cameraSystem = world.getGlobalSystem<CameraSystem>();
	cameraSystem->setUp(glm::vec3(-1.f, 0, 0));
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

#endif
