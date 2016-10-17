
#include <GL/glew.h>

#include "DrawLoop.h"


using namespace Tactics;


Systems::DrawLoop::DrawLoop(GLFWwindow * wind) : myWindow(wind) {
	glfwGetWindowSize(wind, reinterpret_cast<int*>(&width), reinterpret_cast<int*>(&height));
}


void Systems::DrawLoop::handle(const DrawEvent & evt) {
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (unsigned int i = 0; i < systems.size(); i++) {
		systems[i]->run(*getWorld());
	}
	glfwSwapBuffers(myWindow);
}
