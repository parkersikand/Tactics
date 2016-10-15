

#include "WindowManager.h"
#include "DrawLoop.h"
#include "CameraSystem.h"

#include "ThreadedSystem.h"

#include "GLFW/glfw3.h"

using namespace Tactics;


WindowManager::~WindowManager() {
	glfwDestroyWindow(myWindow);
}


void WindowManager::onRegister(ECS::RunnableWorld & world) {
	// add top level drawing
	Systems::DrawEventShooter * des = new Systems::DrawEventShooter;
	world.addRunnableGlobalSystem(*des);

	Systems::DrawLoop * drawLoop = new Systems::DrawLoop(myWindow);
	world.addGlobalSystem(*drawLoop);

	CameraSystem * cameraSystem = new CameraSystem;
	world.addGlobalSystem(*cameraSystem);
}


GLFWwindow * WindowManager::initWindow(std::string title, int width, int height) {
	GLFWwindow * window;
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		char wait = getchar();
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		// Problem: glewInit failed, something is seriously wrong.
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		char wait = getchar();
		glfwTerminate();
	}
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f); // 0 0 102 0

	// enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// setup key input
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, &WindowManager::_key_handle);
	
	// TODO mouse input
	glfwSetScrollCallback(window, &WindowManager::_handleScroll);

	// create a VAO and just kinda leave it for now
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	return window;
} // WindowManager::initWindow


void WindowManager::run(ECS::RunnableWorld & world) {
	glfwPollEvents();
	if (glfwWindowShouldClose(myWindow)) {
		stop();
		// shutdown everything
		world.stop();
	}
}

