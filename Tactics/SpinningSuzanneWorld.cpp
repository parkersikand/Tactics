
#include "SpinningSuzanne.h"

#include "BasicDrawSystem.h"
#include "EventedSystem.h"
#include "OBJLoader.h"
#include "TimedFiringSystem.h"

using namespace Tactics;
using namespace Tactics::ECS;

struct UpdateEvent : public ECS::Event {};

struct SuzanneUpdater : Tactics::EventedSystem<UpdateEvent> {
	void run() {
		modelTransformPtr->transform = glm::rotate(modelTransformPtr->transform, glm::radians(1.f), glm::vec3(1.f, 1.f, 1.f));
	}

	Components::ModelTransform * modelTransformPtr;
};

struct UpdateFiring : public Tactics::TimedFiringSystem<UpdateEvent> {
	UpdateFiring() : Tactics::TimedFiringSystem<UpdateEvent>(30) {};
};

void Tactics::Worlds::SpinningSuzanneWorld::setup() {
	BasicWorld::setup();

	// suzanne
	EntityHdl suzanne = newEntity();
	addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem>>(suzanne);
	auto * s3d = getComponent<Components::CObject3D>(suzanne);
	LoadObj(s3d, "assets/models/suzanne.obj");
	auto * modelTransform = getComponent<Components::ModelTransform>(suzanne);
	auto * suzColor = addComponent<Components::Colored3D>(suzanne);
	Components::Colored3DHelper::SingleColor(suzColor, s3d, glm::vec3(1.f, 0.f, 0.f));

	// updater
	auto * suzanneUpdater = createManagedSystem<SuzanneUpdater>();
	suzanneUpdater->modelTransformPtr = modelTransform;
	
	// firing system
	auto * fire = createManagedSystem<UpdateFiring>();
	addRunnableSystem(*fire);
}

