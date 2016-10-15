
#include "common.h"

#include "LineCollision.h"

#include "Program.h"
#include "SkeletalAnimation.h"
#include "BasicWorld.h"

#include "lodepng.h"

#include <iostream>

using namespace Tactics;


LineCollision::Systems::LineCollisionDetector::LineCollisionDetector() {
	// load program
	// use default vertex shader
	programId = Util::ShaderProgramHelper::LoadShaders("shaders/toon1/vertex.glsl", "shaders/ray/fragment.glsl");
}


void LineCollision::Systems::LineCollisionDetector::handle(const ECS::Events::EntityDestroyedEvent & evt) {
	for (auto it = source_rays.begin(); it != source_rays.end(); it++) {
		if (*it == evt.handle) {
			it = source_rays.erase(it);
		}
	}
}


void LineCollision::Systems::LineCollisionDetector::run(ECS::World & world) {
	auto mask = (ECS::MaskBuilder()).set<LineCollision::Components::LineCollisionSource>().mask();

	// for every source ray, fire and detect all targets
	for (auto sourceHdl : world.filterMaskIncludes(mask)) {
		auto * pos = world.getComponent<::Components::Position3D<>>(sourceHdl);
		auto * ray = world.getComponent<LineCollision::Components::LineCollisionRay>(sourceHdl);
		if (pos && ray) {
			fireRay(*pos, *ray);
		}
	}
}


void LineCollision::Systems::LineCollisionDetector::fireRay(
	const ::Components::Position3D<> & pos,
	Components::LineCollisionRay ray
	) 
{
	auto mask = (ECS::MaskBuilder()).set<Components::LineCollisionTarget>().mask();
	
	// for each target, perform detection
	for (auto targetHdl : getWorld()->filterMaskIncludes(mask)) {
		if (ray.numHits == 0) continue;

		if (fireRay(pos, ray, targetHdl)) {
			ray.numHits--;
		}
	}
} // fireRay


//glm::vec4 unsignedInt2vec4(unsigned int i) {
//}


// Issue draw calls for line collision
void LineCollision::Systems::LineCollisionDetector::LineCollisionDraw(
	const ::Components::Position3D<> & pos,
	Components::LineCollisionRay ray,
	ECS::EntityHdl targetHdl
	) 
{
	// set ray camera info
	auto viewU = glGetUniformLocation(programId, "view_matrix");
	auto projectionU = glGetUniformLocation(programId, "projection_matrix");

	glm::vec3 posv(pos.x, pos.y, pos.z);
	glm::mat4 vm = glm::lookAt(posv, posv + ray.direction, glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(viewU, 1, GL_FALSE, &vm[0][0]);

//	glm::mat4 pm = glm::perspective(glm::radians(45.0f), 4.f / 3.f, 0.01f, 1000.f);
	glm::mat4 pm = glm::perspective(glm::radians(45.0f), 1.f, 0.01f, 1000.f);
	glUniformMatrix4fv(projectionU, 1, GL_FALSE, &pm[0][0]);

	auto isSkeletalU = glGetUniformLocation(programId, "isSkeletal");
	auto modelTransformU = glGetUniformLocation(programId, "model_transform");
	auto objectIdU = glGetUniformLocation(programId, "objectId");

	Tactics::Components::ModelTransform * modelTransform;
	Tactics::Components::Position3D<> * location;
	Tactics::Components::CObject3D * o3dPtr;
	glm::mat4 modelTransformMatrix;

	// draw other objects
	auto objs = getWorld()->filterMaskIncludes((ECS::MaskBuilder()).set<Components::LineCollisionTarget>().mask());
	for (auto hdl : objs) {
		glm::ivec4 objId;
		if (hdl == targetHdl) {
			objId = glm::ivec4(255, 255, 255, 255);
		}
		else {
			objId = glm::ivec4(100, 100, 100, 100);
		}
		//glUniform4uiv(objectIdU, 1, &objId[0]);
		glUniform1ui(objectIdU, hdl == targetHdl ? 1 : 0);

		// grab and load the model transform
		modelTransform = getWorld()->getComponent<Tactics::Components::ModelTransform>(hdl);
		location = getWorld()->getComponent<Tactics::Components::Position3D<> >(hdl);

		// apply model transform, then translate by Position3D 
		modelTransformMatrix =
			glm::translate(glm::mat4(1), glm::vec3(location->x, location->y, location->z)) * modelTransform->transform;
		//auto modelTransformU = glGetUniformLocation(programId, "model_transform");
		glUniformMatrix4fv(modelTransformU, 1, GL_FALSE, &modelTransformMatrix[0][0]);

		// resend uniforms
		glUniform1ui(isSkeletalU, GL_FALSE);

		o3dPtr = getWorld()->getComponent<Tactics::Components::CObject3D>(hdl);

		// check for SkeletalAnimation
		/*
		auto * sa = getWorld()->getComponent<Tactics::Components::SkeletalAnimation>(hdl);
		if (sa != nullptr) {

			// TODO abstract this away into AnimationSystem
			// update time
			double currentTimeMS = glfwGetTime() * 1000;
			if (currentTimeMS - sa->anim_start > sa->animations[sa->currentAnimation].keyFrames.size() * sa->tick_ms) {
				sa->anim_start = currentTimeMS;
			}
			SkeletalAnimation::SkeletalAnimationHelper::InterpolateBoneTransforms(sa, currentTimeMS - sa->anim_start);

			// set flag
			glUniform1ui(isSkeletalU, GL_TRUE);

			// update matrices
			for (unsigned int i = 0; i < sa->bones.size(); i++) {
				auto boneMatU = glGetUniformLocation(programId, ("object_bones[" + std::to_string(i) + "]").c_str());
				glUniformMatrix4fv(boneMatU, 1, GL_FALSE, &sa->bones[i].boneSpace[0][0]);

				auto boneTransformU = glGetUniformLocation(programId, ("bone_transforms[" + std::to_string(i) + "]").c_str());
				//auto bt = glm::rotate(glm::mat4(), glm::radians(-90.f), glm::vec3(1,0,0)) * sa->bones[i].translation * sa->bones[i].rotation * sa->bones[i].scale;
				auto bt = sa->bones[i].boneSpace * sa->bones[i].translation * sa->bones[i].rotation * sa->bones[i].scale; // * sa->bones[i].boneSpace;
				//bt = sa->bones[i].translation * sa->bones[i].rotation * sa->bones[i].scale;
				glUniformMatrix4fv(boneTransformU, 1, GL_FALSE, &bt[0][0]);
			}

			glBindBuffer(GL_ARRAY_BUFFER, sa->vertexBoneInfoVBO);
			glEnableVertexAttribArray(4);
			glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(Tactics::Components::SkeletalAnimation::VertexBoneInfo), (void *)0);
			glEnableVertexAttribArray(5);
			glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(Tactics::Components::SkeletalAnimation::VertexBoneInfo), (void *)1);
			glEnableVertexAttribArray(6);
			glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(Tactics::Components::SkeletalAnimation::VertexBoneInfo), (void *)2);
			glEnableVertexAttribArray(7);
			glVertexAttribIPointer(7, 1, GL_UNSIGNED_INT, sizeof(Tactics::Components::SkeletalAnimation::VertexBoneInfo), (void *)3);
			glEnableVertexAttribArray(8);
			glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(Tactics::Components::SkeletalAnimation::VertexBoneInfo), (void *)4);
			glEnableVertexAttribArray(9);
			glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(Tactics::Components::SkeletalAnimation::VertexBoneInfo), (void *)8);
			glEnableVertexAttribArray(10);
			glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(Tactics::Components::SkeletalAnimation::VertexBoneInfo), (void *)12);
			glEnableVertexAttribArray(11);
			glVertexAttribPointer(11, 1, GL_FLOAT, GL_FALSE, sizeof(Tactics::Components::SkeletalAnimation::VertexBoneInfo), (void *)16);
		}
		*/

		// load vertex data
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, o3dPtr->vxBufId);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		glDrawArrays(o3dPtr->drawMode, 0, o3dPtr->count);
		glDisableVertexAttribArray(0);

	} // for other objects
}


bool LineCollision::Systems::LineCollisionDetector::fireRay(
	const ::Components::Position3D<> & pos,
	Components::LineCollisionRay ray,
	ECS::EntityHdl targetHdl,
	bool fireEvent) 
{
	// get current program
	GLint oldProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);
	
	// switch to ray program
	glUseProgram(programId);

	// clear errors
	glClearErrors();

	// generate and bind back buffers
	
	GLuint fbo, render_buf, depth_buffer;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// color buffer
	glGenRenderbuffers(1, &render_buf);
	glBindRenderbuffer(GL_RENDERBUFFER, render_buf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, bufferAccuracy, bufferAccuracy);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, render_buf);
	
	// depth buffer
	glGenRenderbuffers(1, &depth_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, bufferAccuracy, bufferAccuracy);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);

	auto err = glGetError();
	if (err == GL_INVALID_ENUM) {
		std::cout << "LineCollision::fireRay glRenderbufferStorage INVALID_ENUM" << std::endl;
		assert(false);
	}
	else if (err) {
		std::cout << "LineCollision::fireRay OpenGL error" << std::endl;
		assert(false);
	}
	
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer incomplete" << std::endl;
		assert(false);
	}

	// set up drawing buffer
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	// do drawing /////////////////////////////////////////////////////////////////////
#ifdef TESTING
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // draw to a window for debugging
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	LineCollisionDraw(pos, ray, targetHdl);
	auto * world = getWorld();
	auto * bworld = dynamic_cast<Tactics::Worlds::BasicWorld *>(world);
	glfwSwapBuffers(bworld->getWindow());
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, bufferAccuracy, bufferAccuracy);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearErrors();
	LineCollisionDraw(pos, ray, targetHdl);
	err = glGetError();
	if (err) {
		std::cout << "LineCollisionDetector drawing error" << std::endl;
	}
	glClearErrors();
	///////////////////////////////////////////////////////////////////////////////////////

	// read back pixels
	std::vector<std::uint8_t> centerPixel(4);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glClearErrors();
	glReadPixels(bufferAccuracy / 2, bufferAccuracy / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &centerPixel[0]);

	// read whole image for debugging
#ifdef TESTING
	std::vector<std::uint8_t> data(bufferAccuracy * bufferAccuracy * 4);
	glReadPixels(0, 0, bufferAccuracy, bufferAccuracy, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
	// export png
	auto pngerror = lodepng::encode("LineCollisionDebug.png", data, bufferAccuracy, bufferAccuracy);
	if (pngerror) {
		std::cout << "Lodepng error" << std::endl;
		assert(false);
	}
#endif

	// check for reading errors
	err = glGetError();
	if (err == GL_INVALID_ENUM) {
		std::cout << "OpenGL read error INVALID_ENUM" << std::endl;
		assert(false);
	}
	else if (err == GL_INVALID_OPERATION) {
		std::cout << "OpenGL read error INVALID_OPERATION" << std::endl;
		assert(false);
	}

	// cleanup
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &render_buf);
	glDeleteRenderbuffers(1, &depth_buffer);

	// restore program
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(oldProgram);

	// TODO work out this design better
	/*
	if (fireEvent) {
		LineCollision::Events::LineCollisionEvent lce;
		lce.target = targetHdl;
		lce.source = pos;
		lce.ray = ray;
	}
	*/

	// check center pixel
	return centerPixel[0] == 255 && centerPixel[1] == 255 && centerPixel[2] == 255;
	
} // fireRay

/*

 * * * * *
 * * * * *
 * * * * *
 * * * * *
 * * * * *
 
 */