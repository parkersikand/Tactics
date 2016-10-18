
#include "common.h"

#include "LineCollision.h"

#include "Program.h"
#include "SkeletalAnimation.h"
#include "BasicWorld.h"
#include "CameraSystem.h"

#include "lodepng.h"

#include <iostream>

using namespace Tactics;


LineCollision::Systems::LineCollisionDetector::LineCollisionDetector() {
	// load program
	// use default vertex shader
	programId = Util::ShaderProgramHelper::LoadShaders("shaders/toon1/vertex.glsl", "shaders/ray/fragment.glsl");
	//programId = Util::ShaderProgramHelper::LoadShaders("shaders/toon1/vertex.glsl", "shaders/toon1/fragment.glsl");
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

/*
// fire ray from source and hit all targets
void LineCollision::Systems::LineCollisionDetector::fireRay(
	ECS::EntityHdl source
	) 
{
	auto * pos = getWorld()->getComponent<Tactics::Components::Position3D<>>(source);
	auto * ray = getWorld()->getComponent<LineCollision::Components::LineCollisionRay>(source);
	
	auto mask = (ECS::MaskBuilder()).set<Components::LineCollisionTarget>().mask();

	int hits = ray->numHits;

	// for each target, perform detection
	// TODO order by depth
	for (auto targetHdl : getWorld()->filterMaskIncludes(mask)) {
		if (hits == 0) break;

		if (fireRay(pos, ray, targetHdl)) {
			hits--;
		}
	}

}
*/

void LineCollision::Systems::LineCollisionDetector::fireRay(
	const ::Components::Position3D<> & pos,
	Components::LineCollisionRay ray
	) 
{
	auto mask = (ECS::MaskBuilder()).set<Components::LineCollisionTarget>().mask();
	
	// for each target, perform detection
	for (auto targetHdl : getWorld()->filterMaskIncludes(mask)) {
		if (ray.numHits == 0) break;

		if (fireRay(pos, ray, targetHdl)) {
			ray.numHits--;
		}
	}
} // fireRay


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
	auto lookat = posv + ray.direction;
	glm::mat4 vm = glm::lookAt(posv, lookat, glm::vec3(0.f, 1.f, 0.f));
	glUniformMatrix4fv(viewU, 1, GL_FALSE, &vm[0][0]);

    // get z-values from camera system
	auto * cameraSystem = getWorld()->getGlobalSystem<CameraSystem>();
	glm::mat4 pm = glm::perspective(glm::radians(45.0f), 1.f, cameraSystem->getZNear(), cameraSystem->getZFar());
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

		glUniform1ui(objectIdU, hdl == targetHdl ? 1 : 0);

		// grab and load the model transform
		modelTransform = getWorld()->getComponent<Tactics::Components::ModelTransform>(hdl);
		location = getWorld()->getComponent<Tactics::Components::Position3D<> >(hdl);

		// apply model transform, then translate by Position3D 
		modelTransformMatrix =
			glm::translate(glm::mat4(1), glm::vec3(location->x, location->y, location->z)) * modelTransform->transform;
		glUniformMatrix4fv(modelTransformU, 1, GL_FALSE, &modelTransformMatrix[0][0]);

		// resend uniforms
		glUniform1ui(isSkeletalU, GL_FALSE);

		o3dPtr = getWorld()->getComponent<Tactics::Components::CObject3D>(hdl);

		// check for SkeletalAnimation
		
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

		// load vertex data
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, o3dPtr->vxBufId);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		// load normal data
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, o3dPtr->normBufId);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		glDrawArrays(o3dPtr->drawMode, 0, o3dPtr->count);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(2);

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


float LineCollision::Systems::LineCollisionDetector::linearizeDepth(float d) {
	auto * cameraSystem = getWorld()->getGlobalSystem<CameraSystem>();
	float zNear = cameraSystem->getZNear();
	float zFar = cameraSystem->getZFar();
	return (2 * zNear) / (zFar + zNear - d * (zFar - zNear));
}


LineCollision::Systems::LineCollisionDetector::Result 
LineCollision::Systems::LineCollisionDetector::genericCast(ECS::EntityHdl source, ECS::EntityHdl target, int value, void * dataPtr) {
	auto * pos = getWorld()->getComponent<Tactics::Components::Position3D<>>(source);
	auto  *ray = getWorld()->getComponent<LineCollision::Components::LineCollisionRay>(source);
	
	glClearErrors();

	glUseProgram(programId);

	auto e = glGetError();
	while (e != GL_NO_ERROR) e = glGetError();

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// check for reading errors
	auto err = glGetError();
	if (err == GL_INVALID_ENUM) {
		std::cout << "OpenGL error INVALID_ENUM" << std::endl;
		assert(false);
	}
	else if (err == GL_INVALID_OPERATION) {
		std::cout << "OpenGL error INVALID_OPERATION" << std::endl;
		assert(false);
	}
	else if (err) {
		std::cout << "OpenGL error" << std::endl;
		assert(false);
	}
	glClearErrors();

	GLuint colorBuffer;
	glGenRenderbuffers(1, &colorBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, bufferAccuracy, bufferAccuracy);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);

	// check for reading errors
	err = glGetError();
	if (err == GL_INVALID_ENUM) {
		std::cout << "OpenGL error INVALID_ENUM" << std::endl;
		assert(false);
	}
	else if (err == GL_INVALID_OPERATION) {
		std::cout << "OpenGL error INVALID_OPERATION" << std::endl;
		assert(false);
	}
	glClearErrors();

	GLuint normalBuffer;
	glGenRenderbuffers(1, &normalBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, normalBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, bufferAccuracy, bufferAccuracy);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, normalBuffer);

	// check for reading errors
	err = glGetError();
	if (err == GL_INVALID_ENUM) {
		std::cout << "OpenGL error INVALID_ENUM" << std::endl;
		assert(false);
	}
	else if (err == GL_INVALID_OPERATION) {
		std::cout << "OpenGL error INVALID_OPERATION" << std::endl;
		assert(false);
	}
	glClearErrors();

	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, bufferAccuracy, bufferAccuracy);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

	// check for reading errors
	err = glGetError();
	if (err == GL_INVALID_ENUM) {
		std::cout << "OpenGL error INVALID_ENUM" << std::endl;
		assert(false);
	}
	else if (err == GL_INVALID_OPERATION) {
		std::cout << "OpenGL error INVALID_OPERATION" << std::endl;
		assert(false);
	}
	glClearErrors();

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, drawBuffers);

#ifdef TESTING
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// draw to a window for debugging
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	LineCollisionDraw(*pos, *ray, target);
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
	LineCollisionDraw(*pos, *ray, target);

	std::vector<std::uint8_t> centerPixelColor(4);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glReadPixels(bufferAccuracy / 2, bufferAccuracy / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &centerPixelColor[0]);

#ifdef TESTING
	std::vector<std::uint8_t> dbgColor(bufferAccuracy * bufferAccuracy * 4);
	glReadPixels(0, 0, bufferAccuracy, bufferAccuracy, GL_RGBA, GL_UNSIGNED_BYTE, &dbgColor[0]);
	auto dbgColorPNGErr = lodepng::encode("LineCollider_ColorDebug.png", dbgColor, bufferAccuracy, bufferAccuracy);
#endif

	std::vector<std::uint8_t> centerPixelNormal(4);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glReadPixels(bufferAccuracy / 2, bufferAccuracy / 2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &centerPixelNormal[0]);

#ifdef TESTING
	std::vector<std::uint8_t> dbgNormal(bufferAccuracy * bufferAccuracy * 4);
	glReadPixels(0, 0, bufferAccuracy, bufferAccuracy, GL_RGBA, GL_UNSIGNED_BYTE, &dbgNormal[0]);
	//std::vector<std::uint8_t> dbgNormalPng(bufferAccuracy * bufferAccuracy * 4);
	//for (unsigned int i = 0, j = 0; i < dbgNormal.size(); i+= 3, j += 4) {
	//	dbgNormalPng[j] = dbgNormal[i];
	//	dbgNormalPng[j + 1] = dbgNormal[i+1];
	//	dbgNormalPng[j + 2] = dbgNormal[i+2];
	//	dbgNormalPng[j + 3] = 255;
	//}
	//auto dbgNormalPNGErr = lodepng::encode("LineCollider_NormalDebug.png", dbgNormalPng, bufferAccuracy, bufferAccuracy);
	auto dbgNormalPNGErr = lodepng::encode("LineCollider_NormalDebug.png", dbgNormal, bufferAccuracy, bufferAccuracy);
#endif

	float depth;
	glReadBuffer(GL_DEPTH_ATTACHMENT);
	glReadPixels(bufferAccuracy / 2, bufferAccuracy / 2, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
	depth = linearizeDepth(depth);

#ifdef TESTING
	std::vector<float> dbgDepth(bufferAccuracy * bufferAccuracy);
	glReadPixels(0, 0, bufferAccuracy, bufferAccuracy, GL_DEPTH_COMPONENT, GL_FLOAT, &dbgDepth[0]);
	// format pixels
	std::vector<std::uint8_t> dbgDepthPng(bufferAccuracy * bufferAccuracy * 4);
	for (unsigned int i = 0; i < dbgDepth.size(); i++) {
		dbgDepthPng[i * 4] = linearizeDepth(dbgDepth[i]) * 255;
		dbgDepthPng[i * 4 + 1] = linearizeDepth(dbgDepth[i]) * 255;
		dbgDepthPng[i * 4 + 2] = linearizeDepth(dbgDepth[i]) * 255;
		dbgDepthPng[i * 4 + 3] = 255;
	}
	auto dbgDepthPNGErr = lodepng::encode("LineCollider_DepthDebug.png", dbgDepthPng, bufferAccuracy, bufferAccuracy);
#endif

	switch (value) {
	case 1:
		*reinterpret_cast<std::vector<std::uint8_t>*>(dataPtr) = centerPixelColor;
		break;
	case 2:
		*reinterpret_cast<std::vector<std::uint8_t>*>(dataPtr) = centerPixelNormal;
		break;
	case 3:
		*reinterpret_cast<float*>(dataPtr) = depth;
		break;
	}

	glDeleteRenderbuffers(1, &colorBuffer);
	glDeleteRenderbuffers(1, &normalBuffer);
	glDeleteRenderbuffers(1, &depthBuffer);
	glDeleteFramebuffers(1, &fbo);

	Result result;
	
	result.color.r = centerPixelColor[0] / 255;
	result.color.g = centerPixelColor[1] / 255;
	result.color.b = centerPixelColor[2] / 255;
	result.color.a = centerPixelColor[3] / 255;

	result.normal.x = centerPixelNormal[0] / 255;
	result.normal.y = centerPixelNormal[1] / 255;
	result.normal.z = centerPixelNormal[2] / 255;

	result.depth = depth;

	return result;
} // genericCast


glm::vec4 LineCollision::Systems::LineCollisionDetector::color(ECS::EntityHdl source, ECS::EntityHdl target) {
	std::vector<std::uint8_t> data(4);
	genericCast(source, target, 1, &data);
	return glm::vec4(data[0] / 255, data[1] / 255, data[2] / 255, data[3] / 255);
} // color()


glm::vec3 LineCollision::Systems::LineCollisionDetector::normal(ECS::EntityHdl source, ECS::EntityHdl target) {
	std::vector<std::uint8_t> data(4);
	genericCast(source, target, 2, &data);
	return glm::vec3(data[0] / 255, data[1] / 255, data[2] / 255);
} // normal()


float LineCollision::Systems::LineCollisionDetector::depth(ECS::EntityHdl source, ECS::EntityHdl target) {
	std::vector<float> data;
	genericCast(source, target, 3, &data);
	return data[0];
} // depth


LineCollision::Systems::LineCollisionDetector::Result
LineCollision::Systems::LineCollisionDetector::castResult(ECS::EntityHdl source, ECS::EntityHdl target) {
	return genericCast(source, target, 0, nullptr);
}
