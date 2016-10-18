

#include "TexturedCubeWorld.h"

#include "BasicDrawSystem.h"
#include "OBJLoader.h"
#include "FBX.h"
#include "CameraSystem.h"

using namespace Tactics;
using namespace Tactics::ECS;

void TexturedCubeWorld::setup() {
	Tactics::Worlds::BasicWorld::setup();

	// load custom camera
	
	EntityHdl camera = newEntity();
	auto * cameraPos = addComponent<Components::Position3D<>>(camera);
	cameraPos->y = 5.f;
	cameraPos->x = 1.f;
	auto * cameraComp = addComponent<Components::CameraComponent>(camera);
	//cameraComp->projectionType = cameraComp->ISOMETRIC;
	cameraComp->lookAt = glm::vec3(0.0);
	Tactics::Events::CameraChangedEvent cce;
	cce.newCamera = camera;
	ECS::EventDispatcher::postEvent(cce);
	

	// textured cube
	EntityHdl cube = newEntity();
	addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem> >(cube);
	std::vector<glm::vec3> vx, norm;
	std::vector<glm::vec2> uvs;
	LoadObj("assets/models/correctCube.obj", vx, uvs, norm, true);
	auto * cube3d = addComponent<Components::CObject3D>(cube);
	Components::CObject3DHelper::setData(cube3d, vx, uvs, norm);
	Components::CObject3DHelper::loadTexture2Dpng(cube3d, "assets/models/grass1.png");
	//Components::CObject3DHelper::loadTexture2Dpng(cube3d, "assets/models/labeledCubeTexture.png");
	auto * cube_pos = addComponent<Components::Position3D<> >(cube);
	cube_pos->x = 0;
	cube_pos->y = 0;
	cube_pos->z = 0;

	// load a second cube from FBX
	EntityHdl cube2 = newEntity();
	addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem> >(cube2);
	auto * cube2_3d = addComponent<Components::CObject3D>(cube2);
	Util::FBX::LoadMesh("assets/models/correctCube.fbx", cube2_3d, true);
	//Components::CObject3DHelper::loadTexture2Dpng(cube3d, "assets/models/grass1.png");
	// re use texture id
	cube2_3d->texId = cube3d->texId;
	auto * cube2_pos = addComponent<Components::Position3D<> >(cube2);
	cube2_pos->x = 2.f;
	cube2_pos->y = 0.f;
	cube2_pos->z = 2.f;
}
