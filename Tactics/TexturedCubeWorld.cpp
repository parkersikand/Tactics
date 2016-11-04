

#include "TexturedCubeWorld.h"

#include "BasicDrawSystem.h"
#include "OBJLoader.h"
#include "FBX.h"
#include "CameraSystem.h"
#include "assimp_loader.h"

using namespace Tactics;
using namespace Tactics::ECS;

void TexturedCubeWorld::setup() {
	Tactics::Worlds::BasicWorld::setup();

	// load custom camera
	
	EntityHdl camera = newEntity();
	auto * cameraPos = addComponent<Components::Position3D<>>(camera);
	cameraPos->y = 20.f;
	cameraPos->x = 20.f;
	cameraPos->z = 20.f;
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

	// third cube, rotated 90 on  x
	EntityHdl cube3 = newEntity();
	addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem>>(cube3);
	auto * cube3_3d = getComponent<Components::CObject3D>(cube3);
	Util::FBX::LoadMesh("assets/models/correctCube.fbx", cube3_3d, true);
	cube3_3d->texId = cube3d->texId;
	auto * cube3pos = addComponent<Components::Position3D<>>(cube3);
	cube3pos->x = 4.f;
	cube3pos->z = 4.f;
	auto * cube3_transform = getComponent<Components::ModelTransform>(cube3);
	cube3_transform->transform = glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));


	// fourth cube, rotated 90 on x, then 90 on z
	EntityHdl cube4 = newEntity();
	addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem>>(cube4);
	auto * cube4_3d = getComponent<Components::CObject3D>(cube4);
	Util::FBX::LoadMesh("assets/models/correctCube.fbx", cube4_3d, true);
	cube4_3d->texId = cube3d->texId;
	auto * cube4pos = addComponent<Components::Position3D<>>(cube4);
	cube4pos->x = 4.f;
	cube4pos->z = 6.f;
	auto * cube4_transform = getComponent<Components::ModelTransform>(cube4);
	cube4_transform->transform = 
		glm::rotate(glm::rotate(glm::mat4(1.f), glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f)), glm::radians(90.f), glm::vec3(0.f,0.f,1.f));


	// load a cube with assimp
	EntityHdl cube5 = newEntity();
	addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem>>(cube5);
	auto * cube5_3d = getComponent<Components::CObject3D>(cube5);
	LoadMesh(cube5_3d, "assets/models/correctCube.obj");
	cube5_3d->texId = cube3d->texId;
	auto * cube5pos = addComponent<Components::Position3D<>>(cube5);
	cube5pos->x = -2.f;
	cube5pos->z = -2.f;

}
