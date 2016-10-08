#include <GL/glew.h>

#include "DrawSystem.h"
#include "DrawLoop.h"

#include "Object3D.h"


using namespace Tactics;

void Systems::BaseAbstractDrawSystem::onRegister(ECS::World & world) {
	auto * drawLoop = world.getGlobalSystem<Systems::DrawLoop>();
	drawLoop->addSystem(*this);
}

/*
void DrawSystem::run(std::vector<ECS::Entity> & entities) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// TODO store this in class
	GLuint modelTransformU = glGetUniformLocation(programId, "model_transform");
	GLuint tiModelTransformU = glGetUniformLocation(programId, "ti_model_transform");
	GLuint useTextureU = glGetUniformLocation(programId, "useTexture");

	// TODO I think we only need to do this once, move outside this function
	//glActiveTexture(GL_TEXTURE0); // set texture slot 0 as active
	GLuint textureHW = glGetUniformLocation(programId, "fragmentTextureSampler");
	//glUniform1i(textureHW, 0); // send value 0 to fragmentTextureSampler uniform

	for (auto & e : entities) {

		glUniform1ui(useTextureU, GL_TRUE);

		// grab and load the model transform
		auto * modelTransform = e.getComponent<Components::ModelTransform>();
		auto * location = e.getComponent<Components::Position3D<> >();
		
		// apply model transform, then translate by Position3D 
		glm::mat4 modelTransformMatrix =
			glm::translate(glm::mat4(1), glm::vec3(location->x, location->y, location->z)) * modelTransform->transform;
		
		glUniformMatrix4fv(modelTransformU, 1, GL_FALSE, &modelTransformMatrix[0][0]);

		// apply transform to normals
		glm::mat4 ti_model_transform = glm::transpose(glm::inverse(modelTransformMatrix));
		glUniformMatrix4fv(tiModelTransformU, 1, GL_FALSE, &ti_model_transform[0][0]);

		auto * o3dPtr = e.getComponent<Components::CObject3D>();
		if (o3dPtr == nullptr) continue;

		// bind texture buffer
		glActiveTexture(GL_TEXTURE0); // set texture slot 0 as active
		glBindTexture(GL_TEXTURE_2D, o3dPtr->texId); // bind cubeTex_ID to active texture
		glUniform1i(textureHW, 0); // send value 0 to fragmentTextureSampler uniform

		// load vertex data
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, o3dPtr->vxBufId);
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
			glUniform1ui(useTextureU, GL_FALSE);
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
	}

	glfwSwapBuffers(myWindow);
}
*/

