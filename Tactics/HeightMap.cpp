

#include <GL/glew.h>

#include "HeightMap.h"

#include "Program.h"

using namespace Tactics;


HeightMap::HeightMap::HeightMapProgramManager * HeightMap::HeightMap::HeightMapProgramManager::Instance = NULL;


HeightMap::HeightMap::HeightMapProgramManager * HeightMap::HeightMap::HeightMapProgramManager::getInstance() {
	if (Instance == NULL) {
		Instance = new HeightMapProgramManager();
	}
	return Instance;
}

HeightMap::HeightMap::HeightMapProgramManager::HeightMapProgramManager() {
	programId = Util::ShaderProgramHelper::LoadShaders("shaders/heightmap/vertex.glsl", "shaders/heightmap/fragment.glsl");
}


HeightMap::HeightMap::HeightMap(unsigned int w, unsigned int h): width(w), height(h) {
	heightMap = std::vector<float>(width * height);
}


HeightMap::HeightMap HeightMap::HeightMap::MakeHeightMap(Tactics::Components::CObject3D * obj, glm::vec2 offset) {
	// get current program
	GLuint oldProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&oldProgram));
	
	// get old framebuffer binding
	GLint drawFboId = 0, readFboId = 0;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId);

	// calculate width and height
	unsigned int width, height;
	float minX, minZ, maxX, maxZ;

	for (auto const & vx : obj->vxData) {
		if (vx.x < minX) minX = vx.x;
		if (vx.z < minZ) minZ = vx.z;
		if (vx.x > maxX) maxX = vx.x;
		if (vx.z > maxZ) maxZ = vx.z;
	}

	width = maxX - minX;
	height = maxZ - minZ;

	// attach height map shader program
	glUseProgram(HeightMapProgramManager::getInstance()->programId);

	// create and bind framebuffer
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// create "color" (height) renderbuffer
	GLuint heightBuffer;
	glGenRenderbuffers(1, &heightBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, heightBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, width, height);

	// attach framebuffer and renderbuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, heightBuffer);

	// specify draw buffers
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	// last minute setup
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, width, height);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT);

	// do drawing ////////////////////////////////////////////////////////////////////

	// set up camera
	auto pm = glm::ortho(minX, maxX, minZ, maxZ, -100.f, 1000.f);
	pm = glm::rotate(pm, glm::radians(90.f), glm::vec3(1.f, 0.f, 0.f));
	
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vxBufId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

	glDrawArrays(obj->drawMode, 0, obj->count);

	glDisableVertexAttribArray(0);

	// restore program
	glUseProgram(oldProgram);

	// restore framebuffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFboId);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
}