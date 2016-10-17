

#include "RedCubeWorld.h"
#include "Entity.h"
#include "OBJLoader.h"
#include "Object3D.h"
#include "BasicDrawSystem.h"

using namespace Tactics;
using namespace Tactics::ECS;

void RedCubeWorld::setup() {
	Worlds::BasicWorld::setup();

	// textured cube
	EntityHdl cube = newEntity();
	addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem> >(cube);
	std::vector<glm::vec3> vx, norm;
	std::vector<glm::vec2> uvs;
	LoadObj("assets/models/correctCube.obj", vx, uvs, norm, true);
	auto * cube3d = addComponent<Components::CObject3D>(cube);
	Components::CObject3DHelper::setData(cube3d, vx, uvs, norm);
	auto * cube_pos = addComponent<Components::Position3D<> >(cube);
	cube_pos->x = 0;
	cube_pos->y = 0;
	cube_pos->z = 0;
	auto * color = addComponent<Components::Colored3D>(cube);
	Components::Colored3DHelper::SingleColor(color, cube3d, glm::vec3(5.f, 0.f, 0.f));
}
