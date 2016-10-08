
#include "Animation.h"

using namespace Tactics;

void Systems::AnimationSystem::addEntity(ECS::EntityHdl entity) {
	entities.push_back(entity);

	ECS::World * world = getWorld();

	// unregister from other system if present
	auto * check = world->getComponent<Components::Animation>(entity);
	if (check != nullptr) {
		check->animationSystem->removeEntity(entity);
	}

	// add component
	auto * anim = world->addComponent<Components::Animation>(entity);
	anim->animationSystem = this;
}

void Systems::AnimationSystem::removeEntity(ECS::EntityHdl entity) {
	ECS::World * world = getWorld();
	auto * anim = world->getComponent<Components::Animation>(entity);
	anim->animationSystem = NULL;
	world->removeComponent<Components::Animation>(entity);
}
