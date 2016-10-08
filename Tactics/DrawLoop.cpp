
#include "DrawLoop.h"

using namespace Tactics;

void Systems::DrawLoop::handle(const DrawEvent & evt) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (unsigned int i = 0; i < systems.size(); i++) {
		systems[i]->run(*getWorld());
	}
	glfwSwapBuffers(myWindow);
}
