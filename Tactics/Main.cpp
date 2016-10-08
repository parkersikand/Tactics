
#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm.hpp>

#include <iostream>
#include <vector>

#include <cstdio>

#include "Entity.h"
#include "System.h"

#include "WindowManager.h"
#include "DrawSystem.h"
#include "Program.h"
#include "Object3D.h"
#include "Util.h"
#include "CameraSystem.h"
#include "KeyInputSystem.h"
#include "Animation.h"
#include "BasicDrawSystem.h"

#include "Level.h"

#include "ObjLoader.h"
#include "lodepng.h"
#include "FBX.h"

#include "MainWorldSelector.h"

using namespace Tactics;
using namespace Tactics::ECS;

// simple rotation animation
class SimpleAnimation : public Systems::AnimationSystem {
public:
	glm::mat4 rot = glm::mat4(1.f);

	void run(ECS::World & world, std::vector<EntityHdl> & entities, double deltaTime) {
		// fast way, apply the same rotation to every object
		rot = glm::rotate(rot, (float)deltaTime, glm::vec3(1, 2, 3));
		for (auto const & e : entities) {
			auto * transform = world.getComponent<Components::ModelTransform>(e);
			transform->transform = rot;
		}
	}
};

#ifdef TESTING
#include <gtest/gtest.h>
#endif

int main(int argc, char **argv) {
#ifdef TESTING
	::testing::InitGoogleTest(&argc, argv);
	int val = RUN_ALL_TESTS();
	getchar();
	return val;
#endif

	// initialize OpenGL
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	srand(0);
	

	// load suzanne
	/*
	EntityHdl suzanne = world.newEntity();
	world.addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem> >(suzanne);
	std::vector<glm::vec3> svx, snorm;
	std::vector<glm::vec2> suvs;
	LoadObj("assets/models/suzanne.obj", svx, suvs, snorm);
	auto * cobj3d = world.addComponent<Components::CObject3D>(suzanne);
	Components::CObject3DHelper::setData(cobj3d, svx, suvs, snorm);
	auto * suz_pos = world.addComponent<Components::Position3D<> >(suzanne);
	suz_pos->x = -1;
	suz_pos->y = 0;
	suz_pos->z = 0;
	auto * suz_col = world.addComponent<Components::Colored3D>(suzanne);
	Components::Colored3DHelper::SingleColor(suz_col, cobj3d, glm::vec3(0.2f, 0.2f, 1.0f));
	*/

	// make a shitton of spinning suzannes
	/*
	for (int i = 0; i < 10; i++) {	
		EntityHdl suz = world.newEntity();
		// TODO instead of overriding a template specialization, use an EntityBuilder interface
		world.addComponent<Components::DrawSystemTag<Systems::BasicDrawSystem> >(suz);
		auto * s3d = world.getComponent<Components::CObject3D>(suz);
		Components::CObject3DHelper::setData(s3d, svx, suvs, snorm);
		// unload the texture since we are not using it
		//glDeleteTextures(1, &s3d->texId);
		//s3d->texId = 0;
		auto * p = world.getComponent<Components::Position3D<> >(suz);
		p->x = rand() % 100 / 5 - 5;
		p->y = rand() % 100 / 5 - 5;
		p->z = rand() % 100 / 5 - 5;
		simpleAnimation.addEntity(suz);
		float r = (float)(rand() % 255) / 255;
		float g = (float)(rand() % 255) / 255;
		float b = (float)(rand() % 255) / 255;
		auto * sc = world.addComponent<Components::Colored3D>(suz);
		glm::vec3 color(r, g, b);
		Components::Colored3DHelper::SingleColor(sc, s3d, color);
	}
	*/

	// make a batch of all the added objects
	//basicDraw.makeBatches();

    // Test simple level
/*	
	int h = 2;
	int w = 3;
	int heightMap[2][3] = { 
		{2,3,2},
		{1,1,1}
	};
	GroundType groundType[2][3] = {
		{Grass, Stone, Grass},
		{Stone, Stone, Stone}
	};
	//Level level(h, w, &heightMap[0][0], &groundType[0][0]);
	//level.load(world);
*/

	MainWorld mainWorld;
	mainWorld.start();

	glfwTerminate();
}
