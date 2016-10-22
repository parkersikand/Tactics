

#include "AnimatedCubeWorld.h"

#include "BasicDrawSystem.h"
#include "FBX.h"
#include "CameraSystem.h"
#include "GuideLines.h"
#include "OBJLoader.h"

using namespace Tactics;
using namespace Tactics::ECS;

void AnimatedCubeWorld::setup() {
	Tactics::Worlds::BasicWorld::setup();

	// add a camera
	EntityHdl camera = newEntity();
	auto * cameraComponent = addComponent<Components::CameraComponent>(camera);
	cameraComponent->lookAt = glm::vec3(0.f, 0.f, 0.f);
	cameraComponent->projectionType = cameraComponent->PERSPECTIVE;
	//	cameraComponent->projectionType = cameraComponent->ISOMETRIC;
	auto * cameraPos = addComponent<Components::Position3D<> >(camera);
	cameraPos->x = 0.f;
	cameraPos->y = 0.f;
	cameraPos->z = 20.f;
	Tactics::Events::CameraChangedEvent cce;
	cce.newCamera = camera;
	EventDispatcher::postEvent(cce);

	// guide lines
	EntityHdl guideLines = newTypedEntity<Entities::GuideLines>();

	// animated cube
	EntityHdl animCube = newEntity();
	addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem>>(animCube);
	auto animCube_mesh = addComponent<Components::CObject3D>(animCube);
	Util::FBX::LoadMesh("assets/models/animCube.fbx", animCube_mesh);
	//Util::FBX::LoadMesh("assets/models/correctCube.fbx", animCube_mesh);
	//Util::FBX::LoadMesh("assets/models/animCubeXYZ.fbx", animCube_mesh);
	//Util::FBX::LoadMesh("assets/models/animCubeXYZ90.fbx", animCube_mesh);
	auto animCube_skeletal = addComponent<Components::SkeletalAnimation>(animCube);
	Util::FBX::LoadSkeletalAnimation("assets/models/animCube.fbx", animCube_skeletal, animCube_mesh);
	//Util::FBX::LoadSkeletalAnimation("assets/models/animCubeXYZ.fbx", animCube_skeletal, animCube_mesh);
	//Util::FBX::LoadSkeletalAnimation("assets/models/animCubeXYZ90.fbx", animCube_skeletal, animCube_mesh);
	animCube_skeletal->tick_ms = 300;
	auto animCube_pos = addComponent<Components::Position3D<>>(animCube);
	animCube_pos->x = 0;
	animCube_pos->y = 0;
	animCube_pos->z = 0;
	//auto animCube_color = addComponent<Components::Colored3D>(animCube);
	//Components::Colored3DHelper::SingleColor(animCube_color, animCube_mesh, glm::vec3(0.5, 0.4, 0.4));
	Components::CObject3DHelper::loadTexture2Dpng(animCube_mesh, "assets/models/cube4panelUVmap.png");

	// test triangle
	EntityHdl tri = newEntity();
	addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem>>(tri);
	std::vector<glm::vec3> tvxs = {
		glm::vec3(-1.f,0.f,0.f),
		glm::vec3(0.f,1.f,0.f),
		glm::vec3(1.f,0.f,0.f)
	};
	auto * tobj = addComponent<Components::CObject3D>(tri);
	Components::CObject3DHelper::setVertexData(tobj, tvxs);
	auto * tcol = addComponent<Components::Colored3D>(tri);
	Components::Colored3DHelper::SingleColor(tcol, tobj, glm::vec3(0.f, 1.f, 0.f));
	auto * pos = addComponent<Components::Position3D<>>(tri);
	
	
}

