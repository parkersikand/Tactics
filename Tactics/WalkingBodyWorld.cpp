

#include "WalkingBodyWorld.h"
#include "BasicDrawSystem.h"
#include "assimp_loader.h"


using namespace Tactics;
using namespace Tactics::ECS;


WalkingBodyWorld::WalkingBodyWorld() {
	addRunnableGlobalSystem(updater);
	EventDispatcher::registerEventHandler<Tactics::Events::KeyDown>(updater);
	EventDispatcher::registerEventHandler<Tactics::Events::KeyUp>(updater);
}


void WalkingBodyWorld::setup() {
	Worlds::BasicWorld::setup();

	// set camera
	setCamera(glm::vec3(20.f, 20.f, 20.f), glm::vec3(0.f));

	// load body
	bodyHdl = newEntity();
	addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem>>(bodyHdl);
	auto * body3d = addComponent<Components::MultiObject3D>(bodyHdl);
	LoadMultiMesh(body3d, "assets/models/body.fbx");
	auto * bmodelTransform = addComponent<Components::ModelTransform>(bodyHdl);
	bmodelTransform->transform = glm::scale(glm::mat4(1.f), glm::vec3(0.1, 0.1, 0.1));
	//bmodelTransform->transform = glm::rotate(bmodelTransform->transform, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
	//bmodelTransform->transform = glm::translate(bmodelTransform->transform, glm::vec3(15.f, 0.f, 15.f));
	std::vector<unsigned int> bidxData;
	for (auto & m : body3d->objects) {
		bidxData.insert(bidxData.end(), m.idxData.begin(), m.idxData.end());
	}
	auto * bskeletal = addComponent<Components::SkeletalAnimation>(bodyHdl);
	LoadSkeletal(bskeletal, bidxData, "assets/models/body.fbx");
	auto * skeletalController = addComponent<Components::SkeletalAnimationController>(bodyHdl);
	skeletalController->skeletal = bskeletal;
	skeletalController->isAnimating = false;

	// send values to input handler
	updater.animationController = skeletalController;
	updater.modelTransform = bmodelTransform;
	updater.baseTransform = bmodelTransform->transform;
	updater.position = getComponent<Components::Position3D<>>(bodyHdl);
}


void WalkingBodyWorld::BodyUpdater::handle(const Tactics::Events::KeyDown & kde) {
	switch (kde.keyCode) {
	case GLFW_KEY_UP:
		walking = true;
		animationController->isAnimating = true;
		animationController->animStart = glfwGetTime();
		break;
	case GLFW_KEY_LEFT:
		angularVelocity = 0.001f;
		break;
	case GLFW_KEY_RIGHT:
		angularVelocity = -0.001f;
		break;
	}
}


void WalkingBodyWorld::BodyUpdater::handle(const Tactics::Events::KeyUp & kue) {
	switch (kue.keyCode) {
	case GLFW_KEY_UP:
		walking = false;
		animationController->isAnimating = false;
		break;
	case GLFW_KEY_LEFT:
	case GLFW_KEY_RIGHT:
		angularVelocity = 0.f;
		break;
	}
}


void WalkingBodyWorld::BodyUpdater::run() {
	facing += angularVelocity;
	modelTransform->transform = glm::rotate(baseTransform, facing, glm::vec3(0.f, 1.f, 0.f));
	if (walking) {
		position->x += sin(facing) * walkSpeed;
		position->z += cos(facing) * walkSpeed;
	}
}
