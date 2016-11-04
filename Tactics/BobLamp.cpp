

#include "BobLamp.h"

#include "BasicDrawSystem.h"
#include "assimp_loader.h"

#include <iostream>

using namespace Tactics;
using namespace Tactics::ECS;

void BobLampWorld::setup() {
	Tactics::Worlds::BasicWorld::setup();

	// set camera
	setCamera(glm::vec3(20.f, 20.f, 20.f), glm::vec3(0.f));

	// load bob
	EntityHdl bob = newEntity();
	addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem>>(bob);
	auto * bob3d = addComponent<Components::MultiObject3D>(bob);
	LoadMultiMesh(bob3d, "assets/models/boblampclean.md5mesh");
	auto * modelTransform = addComponent<Components::ModelTransform>(bob);
	modelTransform->transform = glm::scale(glm::mat4(1.f), glm::vec3(0.2, 0.2, 0.2));
    modelTransform->transform = glm::rotate(modelTransform->transform, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
	modelTransform->transform = glm::translate(modelTransform->transform, glm::vec3(0.f, -10.f, 0.f));
	std::vector<unsigned int> idxData;
	for (auto & m : bob3d->objects) {
		idxData.insert(idxData.end(), m.idxData.begin(), m.idxData.end());
	}
	auto * skeletal = addComponent<Components::SkeletalAnimation>(bob);
	LoadSkeletal(skeletal, idxData, "assets/models/boblampclean.md5mesh");
}
