

#include "AnimatedCubeWorld.h"

#include "BasicDrawSystem.h"
#include "FBX.h"

using namespace Tactics;
using namespace Tactics::ECS;

void AnimatedCubeWorld::setup() {
	Tactics::Worlds::BasicWorld::setup();

	// animated cube
	EntityHdl animCube = newEntity();
	addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem>>(animCube);
	auto animCube_mesh = addComponent<Components::CObject3D>(animCube);
	Util::FBX::LoadMesh("assets/models/animCube.fbx", animCube_mesh);
	//Util::FBX::LoadMesh("assets/models/correctCube.fbx", animCube_mesh);
	auto animCube_skeletal = addComponent<Components::SkeletalAnimation>(animCube);
	Util::FBX::LoadSkeletalAnimation("assets/models/animCube.fbx", animCube_skeletal, animCube_mesh);
	animCube_skeletal->tick_ms = 100;
	auto animCube_pos = addComponent<Components::Position3D<>>(animCube);
	animCube_pos->x = 0;
	animCube_pos->y = 0;
	animCube_pos->z = 0;
	auto animCube_color = addComponent<Components::Colored3D>(animCube);
	Components::Colored3DHelper::SingleColor(animCube_color, animCube_mesh, glm::vec3(0.5, 0.4, 0.4));
}

