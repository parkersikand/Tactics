
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
	
	// suggested to try this to get around access violation in LineCollisionDetector test
	//glewExperimental = GL_TRUE;
	
	// initialize OpenGL
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		char wait = getchar();
		return -1;
	}

	srand(0);

#ifdef TESTING
	::testing::InitGoogleTest(&argc, argv);
	int val = RUN_ALL_TESTS();
	getchar();
	return val;
#endif

	MainWorld mainWorld;
	mainWorld.start();

	glfwTerminate();
}
