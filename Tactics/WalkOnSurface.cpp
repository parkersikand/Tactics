

#include "WalkOnSurface.h"

#include "Event.h"

#include "KeyInputSystem.h"
#include "LineCollision.h"
#include "Util.h"
#include "CameraSystem.h"
#include "OBJLoader.h"
#include "DrawSystem.h"
#include "BasicDrawSystem.h"

#include <gtx/rotate_vector.hpp>

using namespace Tactics;


struct Worlds::WalkOnSurfaceWorld::PlayerController :
	public Tactics::Systems::KeyInputSystem 
{

	float mult = 0.00005;
	float walkSpeed = 0.1;
	float leftRightD = 0.f;
	float upDownD = 0.f;

	glm::vec3 velocity;

	void handle(const Events::KeyDown & kde) {
		switch (kde.keyCode) {
		case GLFW_KEY_W: velocity.z = -1.f; break;
		case GLFW_KEY_A: velocity.x = -1.f; break;
		case GLFW_KEY_S: velocity.z = 1.f; break;
		case GLFW_KEY_D: velocity.x = 1.f; break;
		case GLFW_KEY_LEFT: leftRightD = mult; break;
		case GLFW_KEY_RIGHT: leftRightD = -mult; break;
		case GLFW_KEY_UP: upDownD = -mult; break;
		case GLFW_KEY_DOWN: upDownD = +mult; break;
		}
	}
	
	void handle(const Events::KeyUp & kue) {
		switch (kue.keyCode) {
		case GLFW_KEY_W: velocity.z = 0.f; break;
		case GLFW_KEY_A: velocity.x = 0.f; break;
		case GLFW_KEY_S: velocity.z = 0.f; break;
		case GLFW_KEY_D: velocity.x = 0.f; break;
		case GLFW_KEY_LEFT: leftRightD = 0.f; break;
		case GLFW_KEY_RIGHT: leftRightD = 0.f; break;
		case GLFW_KEY_UP: upDownD = 0.f; break;
		case GLFW_KEY_DOWN: upDownD = 0.f; break;
		}
	}

	void handle(const Events::KeyRepeat & kre) {

	}

	// the player
	ECS::EntityHdl playerHdl;

	// the surface to walk on
	ECS::EntityHdl floorHdl;

	// position of player
	Components::Position3D<> * position;

	// camera component
	Components::CameraComponent * camera;

	// the direction the player is looking
	// looking along -z is 0 degrees
	glm::vec3 looking = glm::vec3(0.f, 0.f, -1.f);
	float upDownRad = 0;
	float leftRightRad = 0;

	// convert looking vector into xz plane direction
	float compassDirectionDegrees() {
		float val = atan(looking.z / looking.x);
		if (looking.x < 0) 
			return glm::pi<float>() + val;
		if (looking.x > 0 && looking.z < 0)
			return 2*glm::pi<float>() + val;
		return val;
	};

	LineCollision::Systems::LineCollisionDetector lcd;


	ECS::EntityHdl sourceHdl;
	void move(glm::vec3 direction) {
		// set direction y to 0
		direction.y = 0;
		
		// create linecast source
		auto * sourcePos = getWorld()->getComponent<Components::Position3D<>>(sourceHdl);
		sourcePos->x = position->x + direction.x;
		sourcePos->y = position->y + 3.f;// +1.f;
		sourcePos->z = position->z + direction.z;
		
		// cast ray straight down
		auto * ray = getWorld()->getComponent<LineCollision::Components::LineCollisionRay>(sourceHdl);
		ray->direction = glm::vec3(0.f, -1.f, 0.f);
		ray->up = glm::vec3(-1.f, 0, 0);

		// make sure floor is a LineCollisionTarget
		getWorld()->addComponent<LineCollision::Components::LineCollisionTarget>(floorHdl);

		// get normal from linecast
		auto result = lcd.castResult(sourceHdl, floorHdl);
		
		// if normal is 0, we are over the edge, dont move
		if (result.normal == glm::vec3(0.f, 0.f, 0.f)) return;
		
		// check slope of normal
		auto slope = lineNormalAngleRad(result.normal, glm::vec3(0.f, 1.f, 0.f));
		if (slope > glm::radians(60.f)) { // too steep, dont move
			return;
		}

		// we are good, update position
		position->x += direction.x;
		position->z += direction.z;
		position->y += glm::length(direction) * (slope / (glm::pi<float>()/2));

	} // move

	void update() {

		//std::cout << position->x << "\t" << position->z << std::endl;

		// update angles
		upDownRad += upDownD;
		leftRightRad += leftRightD;

		// create looking vector based on angles
		glm::vec3 vec(0.f, 0.f, 1.f);
		vec = glm::rotate(vec, upDownRad, glm::vec3(1.f, 0.f, 0.f));
		vec = glm::rotate(vec, leftRightRad, glm::vec3(0.f, 1.f, 0.f));
		looking = vec;

		glm::vec3 directedVelocity;
		directedVelocity.x = sin(leftRightRad);
		directedVelocity.z = cos(leftRightRad);
		directedVelocity = glm::normalize(directedVelocity);
		directedVelocity *= walkSpeed;

		if (glm::length(velocity) > 0) {
//			move(velocity * walkSpeed);
			move(directedVelocity);
		}
		
		// update camera
		camera->lookAt = glm::vec3(position->x, position->y, position->z) + looking;
		Events::CameraChangedEvent cce;
		cce.newCamera = playerHdl;
		ECS::EventDispatcher::postEvent(cce);
	} // update

	// perform setup upon registration
	void onRegister(World &) {
		sourceHdl = getWorld()->newEntity();
		getWorld()->addComponent<Components::Position3D<>>(sourceHdl);
		getWorld()->addComponent<LineCollision::Components::LineCollisionRay>(sourceHdl);
	}

}; // PlayerController


struct PlayerControllerUpdater : public Tactics::ECS::RunnableSystem {
	void run() {
		pc->update();
	}
	Worlds::WalkOnSurfaceWorld::PlayerController * pc;
};


void Worlds::WalkOnSurfaceWorld::setup() {
	BasicWorld::setup(); // super call

	// unregister default input system
	unregisterCameraKeyHandler();

	// create the PlayerController
	playerController = createManagedSystem<PlayerController>();
	ECS::EventDispatcher::registerEventHandler<Events::KeyDown>(*playerController);
	ECS::EventDispatcher::registerEventHandler<Events::KeyUp>(*playerController);

	// add LineCollider
	playerController->lcd = *createManagedSystem<LineCollision::Systems::LineCollisionDetector>();

	// create player
	playerController->playerHdl = newEntity();
	playerController->position = addComponent<Components::Position3D<>>(playerController->playerHdl);
	playerController->position->y = 1.f;
	playerController->camera = addComponent<Components::CameraComponent>(playerController->playerHdl);
	playerController->camera->lookAt = glm::vec3(0.f, 1.f, 1.f);

	// load floor
	playerController->floorHdl = newEntity();
	addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem> >(playerController->floorHdl);
	std::vector<glm::vec3> vx, norm;
	std::vector<glm::vec2> uv;
	LoadObj("assets/models/level1.obj", vx, uv, norm, true);
	auto * floor3d = addComponent<Components::CObject3D>(playerController->floorHdl);
	Components::CObject3DHelper::setData(floor3d, vx, uv, norm);
	auto * floorColor = addComponent<Components::Colored3D>(playerController->floorHdl);
	Components::Colored3DHelper::SingleColor(floorColor, floor3d, glm::vec3(0.6, 0.6, 0.6));
	floorHdl = playerController->floorHdl;
	addComponent<LineCollision::Components::LineCollisionTarget>(playerController->floorHdl);
	auto * floorTransform = getComponent<Components::ModelTransform>(playerController->floorHdl);
	//floorTransform->transform = glm::translate(glm::vec3(0, 5.f, 0));


	// create updater system
	auto * pcu = createManagedSystem<PlayerControllerUpdater>();
	pcu->pc = playerController;
	addRunnableGlobalSystem(*pcu);
}


// returns whether or not we can successfully move
// must be called after setup
bool Worlds::WalkOnSurfaceWorld::testStep1() {
	auto originalPos = *playerController->position;
	playerController->velocity.z = 0.1;
	playerController->move(playerController->velocity);
	auto newPos = *playerController->position;
	return originalPos.x != newPos.x || originalPos.y != newPos.y || originalPos.z != newPos.z;
}

