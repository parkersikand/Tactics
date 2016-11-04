
#include "BasicWorld.h"

#include "KeyInputSystem.h"
#include "Object3D.h"
#include "CameraSystem.h"
#include "WindowManager.h"
#include "BasicDrawSystem.h"
#include "ColoredDrawSystem.h"
#include "common.h"


using namespace Tactics;
using namespace Tactics::ECS;

// a simple key handler to move the camera
	void Worlds::BasicWorld::CameraKeyHandler::handle(const Tactics::Events::ScrollEvent & se) {
		//std::cout << "Scroll" << std::endl;
		World & world = *getWorld();
		auto * pos = world.getComponent<Components::Position3D<> >(camera);
		auto * camInfo = world.getComponent<Components::CameraComponent>(camera);

		glm::vec3 currentPosition(pos->x, pos->y, pos->z);

		// compute the current direction we are looking
		glm::vec3 dir = glm::normalize(camInfo->lookAt - currentPosition);

		// move camera back and forth along direction
		glm::vec3 newPosition = currentPosition + ((float)se.y) * dir;
		pos->x = newPosition.x;
		pos->y = newPosition.y;
		pos->z = newPosition.z;

		// update the camera system
		Tactics::Events::CameraChangedEvent cce;
		cce.newCamera = camera;
		ECS::EventDispatcher::postEvent(cce);
	}

	void Worlds::BasicWorld::CameraKeyHandler::update(int keycode) {
		// rotate the camera, keep it looking at the origin
		World & world = *getWorld();

		auto * cc = world.getComponent<Components::CameraComponent>(camera);
		cc->lookAt = glm::vec3(0, 0, 0);
		cc->projectionType = cc->PERSPECTIVE;

		auto * pos = world.getComponent<Components::Position3D<> >(camera);

		// calculate our distance from origin in XZ plane
		float dist = sqrt(pos->x * pos->x + pos->z * pos->z);
		// calculate our angle WRT origin
		float angle = atan(pos->z / pos->x);
		if (pos->x < 0) angle += (float)PI;
		else if (pos->z < 0 && pos->x > 0) angle += 2 * (float)PI;
		//std::cout << angle * 180 / PI << std::endl;
		// increase or decrease based on input
		if (keycode == GLFW_KEY_LEFT) {
			angle += mult * 1;
			pos->x = dist * cos(angle);
			pos->z = dist * sin(angle);
		}
		else if (keycode == GLFW_KEY_RIGHT) {
			angle -= mult * 1;
			pos->x = dist * cos(angle);
			pos->z = dist * sin(angle);
		}
		else if (keycode == GLFW_KEY_DOWN) {

		}
		else if (keycode == GLFW_KEY_UP) {

		}
		else if (keycode == GLFW_KEY_SPACE) {
			// reset position
			pos->x = 0;
			pos->z = -dist;
		}

		//		std::cout << "X: " << pos->x << " Z: " << pos->z << std::endl;

		// update the camera system
		Tactics::Events::CameraChangedEvent cce;
		cce.newCamera = camera;
		ECS::EventDispatcher::postEvent(cce);
	}


void Tactics::Worlds::BasicWorld::setup() {
	// add systems

	windowManager = createManagedSystem<WindowManager>();
	addRunnableGlobalSystem(*windowManager);

	auto * basicDraw = createManagedSystem<Systems::BasicDrawSystem>();
	
	// add a camera
	// TODO figure this out better, less clunky
	EntityHdl camera = newEntity();
	Components::CameraComponent * cameraComponent = addComponent<Components::CameraComponent>(camera);
	cameraComponent->lookAt = glm::vec3(0.f, 0.f, 0.f);
	cameraComponent->projectionType = cameraComponent->PERSPECTIVE;
	//	cameraComponent->projectionType = cameraComponent->ISOMETRIC;
	Components::Position3D<> * cameraPos = addComponent<Components::Position3D<> >(camera);
	cameraPos->x = 10.0f;
	cameraPos->y = 10.f;
	cameraPos->z = 20.f;
	Events::CameraChangedEvent cce;
	cce.newCamera = camera;
	EventDispatcher::postEvent(cce);
	cameraHdl = camera;

	// add our key handler to update the camera
	//CameraKeyHandler ckh;
	cameraKeyHandler = createManagedSystem<CameraKeyHandler>();
	cameraKeyHandler->camera = camera;
	ECS::EventDispatcher::registerEventHandler<Events::KeyDown>(*cameraKeyHandler);
	ECS::EventDispatcher::registerEventHandler<Events::KeyRepeat>(*cameraKeyHandler);
	ECS::EventDispatcher::registerEventHandler<Events::ScrollEvent>(*cameraKeyHandler);
	//registerSystem(ckh);
}


void Tactics::Worlds::BasicWorld::setCamera(glm::vec3 pos, glm::vec3 lookAt) {
	setCamera(pos, lookAt, glm::vec3(0.f, 1.f, 0.f));
}

void Tactics::Worlds::BasicWorld::setCamera(glm::vec3 pos, glm::vec3 lookAt, glm::vec3 up) {
	auto camera = getCamera();
	Components::CameraComponent * cameraComponent = getComponent<Components::CameraComponent>(camera);
	cameraComponent->lookAt = glm::vec3(0.f, 0.f, 0.f);
	cameraComponent->projectionType = cameraComponent->PERSPECTIVE;
	//	cameraComponent->projectionType = cameraComponent->ISOMETRIC;
	Components::Position3D<> * cameraPos = getComponent<Components::Position3D<> >(camera);
	cameraPos->x = pos.x;
	cameraPos->y = pos.y;
	cameraPos->z = pos.z;
	Events::CameraChangedEvent cce;
	cce.newCamera = camera;
	EventDispatcher::postEvent(cce);
}


GLFWwindow * Tactics::Worlds::BasicWorld::getWindow() {
	return windowManager->getWindow();
}

void Tactics::Worlds::BasicWorld::unregisterCameraKeyHandler() {
	ECS::EventDispatcher::getInstance()->removeEventHandler(*cameraKeyHandler);
	destroyManagedSystem(cameraKeyHandler);
}
