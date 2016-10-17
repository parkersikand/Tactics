
#include "ColoredDrawSystem.h"
#include "Program.h"
#include "Object3D.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

using namespace Tactics;


Systems::ColoredDrawSystem::ColoredDrawSystem() {
	// load glsl program
	programId = Util::ShaderProgramHelper::LoadShaders("shaders/colored/vertex.glsl", "shaders/colored/fragment.glsl");
	viewU = glGetUniformLocation(programId, "view_matrix");
	projectionU = glGetUniformLocation(programId, "projection_matrix");
	modelTransformU = glGetUniformLocation(programId, "model_transform");
}


void Systems::ColoredDrawSystem::run(std::vector<ECS::Entity> & entities) {
	glUseProgram(programId);

	auto vm = glm::lookAt(glm::vec3(0.f, 20.f, 10.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(viewU, 1, GL_FALSE, &vm[0][0]);

	auto pm = glm::perspective(glm::radians(45.f), 4.f / 3.f, 0.01f, 1000.f);
	glUniformMatrix4fv(projectionU, 1, GL_FALSE, &pm[0][0]);
	
	for (auto & e : entities) {

		// grab and load the model transform
		auto * modelTransform = e.getComponent<Components::ModelTransform>();
		auto * location = e.getComponent<Components::Position3D<> >();
		auto * o3dPtr = e.getComponent<Components::CObject3D>();

		// apply model transform, then translate by Position3D 
		glm::mat4 modelTransformMatrix =
			glm::translate(glm::mat4(1), glm::vec3(location->x, location->y, location->z)) * modelTransform->transform;

		glUniformMatrix4fv(modelTransformU, 1, GL_FALSE, &modelTransformMatrix[0][0]);

		// load vertex data
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, o3dPtr->vxBufId);
		GLint nBufferSize;
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &nBufferSize);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		// load uv data
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, o3dPtr->uvBufId);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

		// load normal data
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, o3dPtr->normBufId);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		// check for color component
		auto * col3d = e.getComponent<Components::Colored3D>();
		if (col3d != NULL) {
			glEnableVertexAttribArray(3);
			glBindBuffer(GL_ARRAY_BUFFER, col3d->bufId);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
		}

		// call draw
		glDrawArrays(o3dPtr->drawMode, 0, o3dPtr->count);

		// disable vertex arrays
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);

	} // for entities
}

