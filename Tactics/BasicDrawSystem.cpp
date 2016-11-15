
#include "BasicDrawSystem.h"

#include "Program.h"
#include "CameraSystem.h"
#include "SkeletalAnimation.h"
#include "common.h"

#include <algorithm>


using namespace Tactics;


Systems::BasicDrawSystem::BasicDrawSystem() {
	// load glsl program
	programId = Util::ShaderProgramHelper::LoadShaders("shaders/toon1/vertex.glsl", "shaders/toon1/fragment.glsl");
	
	// use these for debugging
	//programId = Util::ShaderProgramHelper::LoadShaders("shaders/simple/vertex.glsl", "shaders/simple/fragment.glsl");
	//programId = Util::ShaderProgramHelper::LoadShaders("shaders/colored/vertex.glsl", "shaders/colored/fragment.glsl");
	
	glUseProgram(programId);
	viewU = glGetUniformLocation(programId, "view_matrix");
	projectionU = glGetUniformLocation(programId, "projection_matrix");
	hwLightDir = glGetUniformLocation(programId, "lightDir");
	modelTransformU = glGetUniformLocation(programId, "model_transform");
	tiModelTransformU = glGetUniformLocation(programId, "ti_model_transform");
	useTextureU = glGetUniformLocation(programId, "useTexture");
	textureHW = glGetUniformLocation(programId, "fragmentTextureSampler");
	isSkeletalU = glGetUniformLocation(programId, "isSkeletal");
	fogColorU = glGetUniformLocation(programId, "fogParams.fogColor");
	fogDensityU = glGetUniformLocation(programId, "fogParams.fogDensity");
	viewDirectionU = glGetUniformLocation(programId, "viewDirection");
	cameraPosU = glGetUniformLocation(programId, "cameraPos");

	// bind bone uniforms
	for (unsigned int b = 0; b < 64; b++) {
		boneU[b] = glGetUniformLocation(programId, ("bones[" + std::to_string(b) + "]").c_str());
		boneITU[b] = glGetUniformLocation(programId, ("bonesIT[" + std::to_string(b) + "]").c_str());
	}

	// generate batch buffers
	glGenBuffers(1, &vxVBO);
	glGenBuffers(1, &uvVBO);
	glGenBuffers(1, &normVBO);
	glGenBuffers(1, &colorVBO);

	// load billboard program
	billboardProgramId = Util::ShaderProgramHelper::LoadShaders("shaders/billboard/vertex.glsl", "shaders/billboard/fragment.glsl");
	billboardViewU = glGetUniformLocation(billboardProgramId, "view_matrix");
	billboardSizeU = glGetUniformLocation(billboardProgramId, "billboardSize");
	billboardCenterU = glGetUniformLocation(billboardProgramId, "billboardCenter");
	billboardUseTextureU = glGetUniformLocation(billboardProgramId, "useTexture");
	billboardProjectionU = glGetUniformLocation(billboardProgramId, "projection_matrix");
}

struct TexIDSorter {
	bool operator() (ECS::EntityHdl a, ECS::EntityHdl b) {
		Components::CObject3D * aptr = world->getComponent<Components::CObject3D>(a);
		Components::CObject3D * bptr = world->getComponent<Components::CObject3D>(b);
		return aptr->texId < bptr->texId;
	}
	ECS::World * world;
};


Systems::BasicDrawSystem::Batch::Batch() {
	glGenBuffers(1, &vxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, vxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MAX_VERTEX_COUNT, NULL, GL_STATIC_DRAW);
	glGenBuffers(1, &uvVBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * MAX_VERTEX_COUNT, NULL, GL_STATIC_DRAW);
	glGenBuffers(1, &normVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MAX_VERTEX_COUNT, NULL, GL_STATIC_DRAW);
	glGenBuffers(1, &colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MAX_VERTEX_COUNT, NULL, GL_STATIC_DRAW);
}


// batch up all objects to be drawn by this system
void Systems::BasicDrawSystem::makeBatches() {
	std::vector<ECS::EntityHdl> handles = getEntities();
	ECS::World * world = getWorld();

	// clear whatever batches are there
	for (auto const & batch : batches) {
		glDeleteBuffers(1, &vxVBO);
		glDeleteBuffers(1, &uvVBO);
		glDeleteBuffers(1, &normVBO);
		glDeleteBuffers(1, &colorVBO);
	}
	batches.clear();

	std::map<GLuint, std::vector<ECS::EntityHdl> > texHandles;
	for (auto const & hdl : handles) {
		auto * o3d = world->getComponent<Components::CObject3D>(hdl);
		texHandles[o3d->texId].push_back(hdl);
	}

	Components::CObject3D * o3d;
	Components::Colored3D * col;

	for (auto const & tex_hdl : texHandles) {
		// make first batch for this texture
		Batch currentBatch;
		currentBatch.textureId = tex_hdl.first;
		currentBatch.useTexture = currentBatch.textureId > 0;

		for (auto const & hdl : tex_hdl.second) {
			o3d = world->getComponent<Components::CObject3D>(hdl);
			col = world->getComponent<Components::Colored3D>(hdl);
			Batch::ObjectInfo objectInfo;
			objectInfo.handle = hdl;
			objectInfo.count = o3d->count;
			if (currentBatch.vertexCount + o3d->count > Batch::MAX_VERTEX_COUNT) {
				batches.push_back(currentBatch);
				currentBatch = Batch();
				currentBatch.textureId = tex_hdl.first;
				currentBatch.useTexture = currentBatch.textureId > 0;
			}

			// copy data
			unsigned int vertexOffset = currentBatch.vertexCount;
			glBindBuffer(GL_ARRAY_BUFFER, currentBatch.vxVBO);
			glBufferSubData(GL_ARRAY_BUFFER, vertexOffset * sizeof(o3d->vxData[0]), o3d->count * sizeof(glm::vec3), &o3d->vxData[0]);
			glBindBuffer(GL_ARRAY_BUFFER, currentBatch.uvVBO);
			if (o3d->uvData.size()) {
				glBufferSubData(GL_ARRAY_BUFFER, vertexOffset * sizeof(o3d->uvData[0]), o3d->count * sizeof(glm::vec2), &o3d->uvData[0]);
			}
			else {

			}
			glBindBuffer(GL_ARRAY_BUFFER, currentBatch.normVBO);
			glBufferSubData(GL_ARRAY_BUFFER, vertexOffset * sizeof(o3d->normData[0]), o3d->count * sizeof(glm::vec3), &o3d->normData[0]);
			glBindBuffer(GL_ARRAY_BUFFER, currentBatch.colorVBO);
			if (col != nullptr) {
				glBufferSubData(GL_ARRAY_BUFFER, vertexOffset * sizeof(col->data[0]), o3d->count * sizeof(glm::vec3), &col->data[0]);
			}
			else {

			}
			objectInfo.index = currentBatch.objectCount;
			currentBatch.objectCount += 1;
			currentBatch.vertexCount += o3d->count;
			currentBatch.objectInfo.push_back(objectInfo);

			// mark as batched
			auto * bc = world->addComponent<Components::BasicDrawSystemBatch>(hdl);
			bc->isBatched = true;
		}
		batches.push_back(currentBatch);
	}
	
} // Systems::BasicDrawSystem::makeBatches() 


void Systems::BasicDrawSystem::run(std::vector<ECS::Entity> & entities) {
	glUseProgram(programId);
	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	// set global uniforms
	glUniform1ui(isSkeletalU, 0);

	// set camera info
	auto * cameraSystem = getWorld()->getGlobalSystem<CameraSystem>();
	//auto vm = cameraSystem->getViewMatrix();
	auto vm = glm::lookAt(glm::vec3(5, 5, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(viewU, 1, GL_FALSE, &vm[0][0]);
	//auto pm = cameraSystem->getProjectionMatrix();
	auto pm = glm::perspective(glm::radians(45.f), 4.f / 3.f, 0.01f, 1000.f);
	
	glUniformMatrix4fv(projectionU, 1, GL_FALSE, &pm[0][0]);
	glm::vec3 lightDir = glm::normalize(glm::vec3(5.f, 10.f, 5.f));
	
	glUniform3fv(hwLightDir, 1, &lightDir[0]);
	glActiveTexture(GL_TEXTURE0); // set texture slot 0 as active
	glUniform1ui(useTextureU, 0);

	// send fog info
	glUniform4fv(fogColorU, 1, &fogParams.fogColor[0]);
	glUniform1f(fogDensityU, fogParams.fogDensity);

	// draw batches
	unsigned int i = 0;
	for (i = 0; i < batches.size(); i++) {
		Batch batch = batches[i];
		glUniform1ui(useTextureU, batch.useTexture);
		if (i > 0 && batch.textureId != batches[i - 1].textureId) {
			glBindTexture(GL_TEXTURE_2D, batch.textureId);
		}
		// vertex data
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, batch.vxVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
		// load uv data
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, batch.uvVBO);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
		// load normal data
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, batch.normVBO);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
		// load color data
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, batch.colorVBO);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		// draw objects
		unsigned int offset = 0;
		for (auto const & oi : batch.objectInfo) {
			// grab and load the model transform
			auto * modelTransform = getWorld()->getComponent<Components::ModelTransform>(oi.handle);
			auto * location = getWorld()->getComponent<Components::Position3D<> >(oi.handle);

			// apply model transform, then translate by Position3D 
			glm::mat4 modelTransformMatrix =
				glm::translate(glm::mat4(1), glm::vec3(location->x, location->y, location->z)) * modelTransform->transform;

			glUniformMatrix4fv(modelTransformU, 1, GL_FALSE, &modelTransformMatrix[0][0]);
			
			// apply transform to normals
			glm::mat4 ti_model_transform = glm::transpose(glm::inverse(modelTransformMatrix));
			glUniformMatrix4fv(tiModelTransformU, 1, GL_FALSE, &ti_model_transform[0][0]);
//			glUniformMatrix4fv(tiModelTransformU, 1, GL_FALSE, &modelTransformMatrix[0][0]);

			// draw
			glDrawArrays(batch.drawType, offset, oi.count);
			offset += oi.count;
		}
	}
	// disable vertex arrays
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

	// draw unbatched objects
	draw(entities);
}

void Systems::BasicDrawSystem::HandleSkeletal(Components::SkeletalAnimationController * sa, unsigned int offset) {

	
	if (sa->isAnimating) {
		sa->skeletal->BoneTransforms((float)glfwGetTime() - sa->animStart);
	}
	else {
		sa->skeletal->BoneTransforms(0.f);
	}
	

	// set flag
	glUniform1ui(isSkeletalU, GL_TRUE);
	glm::mat3 boneIT;
	// send transforms to shader
	for (unsigned int b = 0; b < sa->skeletal->bones.size(); b++) {
		glUniformMatrix4fv(boneU[b], 1, GL_TRUE, &sa->skeletal->bone_transforms[b][0][0]);
		boneIT = glm::mat3(glm::inverse(sa->skeletal->bone_transforms[b]));
		glUniformMatrix3fv(boneITU[b], 1, GL_FALSE, &boneIT[0][0]);
	}

	auto sz = sizeof(Components::SkeletalAnimation::VertexBoneInfo);

	// send vertex bone info
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, sa->skeletal->vertexBoneInfoVBO);
	glVertexAttribIPointer(4, 4, GL_UNSIGNED_BYTE, sz, (void *)(offset * sz));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sz, (void *)(offset * sz + 4));

}

void Systems::BasicDrawSystem::drawBillboardVarying(ECS::Entity & entity) {
	auto * billboardVarying = entity.getComponent<Components::BillboardVarying>();

	// use billboard program
	glUseProgram(billboardProgramId);

	// diable depth test
	glDepthFunc(GL_ALWAYS);

	// set camera info
	auto * cameraSystem = getWorld()->getGlobalSystem<CameraSystem>();
	auto vm = cameraSystem->getViewMatrix();
	glUniformMatrix4fv(billboardViewU, 1, GL_FALSE, &vm[0][0]);

	//auto ivm = glm::inverse(vm);
	//auto cameraRightWorld = glm::normalize(glm::vec3(ivm[0][0], ivm[0][1], ivm[0][2]));
	//auto cameraUpWorld = glm::normalize(glm::vec3(ivm[1][0], ivm[1][1], ivm[1][2]));
	//glUniform3fv(billboardCameraRightU, 1, &cameraRightWorld[0]);
	//glUniform3fv(billboardCameraUpU, 1, &cameraUpWorld[0]);

	auto pm = cameraSystem->getProjectionMatrix();
	glUniformMatrix4fv(billboardProjectionU, 1, GL_FALSE, &pm[0][0]);

	// send center
	glUniform3fv(billboardCenterU, 1, &billboardVarying->center[0]);

	// send size
	glm::vec2 sz(billboardVarying->width, billboardVarying->height);
	glUniform2fv(billboardSizeU, 1, &sz[0]);
	
	// load 3d data
	auto * o3d = entity.getComponent<Components::CObject3D>();
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, o3d->vxBufId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, o3d->uvBufId);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);

	// check for color component
	auto * col3d = entity.getComponent<Components::Colored3D>();
	if (col3d != NULL) {
		glUniform1ui(billboardUseTextureU, GL_FALSE);
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, col3d->bufId);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	}

	glDrawArrays(o3d->drawMode, 0, o3d->count);

	// restore program
	glUseProgram(programId);
	glDepthFunc(GL_LESS);
}

// draw unbatched objects
void Systems::BasicDrawSystem::draw(std::vector<ECS::Entity> & entities) {
	glClearErrors();
	glUseProgram(programId);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// set camera info
	auto * cameraSystem = getWorld()->getGlobalSystem<CameraSystem>();
	auto vm = cameraSystem->getViewMatrix();
	glUniformMatrix4fv(viewU, 1, GL_FALSE, &vm[0][0]);

	auto vd = cameraSystem->getViewDirection();
	glUniform3fv(viewDirectionU, 1, &vd[0]);

	auto cp = cameraSystem->getCameraPos();
	glUniform3fv(cameraPosU, 1, &cp[0]);

	auto pm = cameraSystem->getProjectionMatrix();
	glUniformMatrix4fv(projectionU, 1, GL_FALSE, &pm[0][0]);

	glm::vec3 lightDir = glm::normalize(glm::vec3(10.f, 10.f, 10.f));
	glUniform3fv(hwLightDir, 1, &lightDir[0]);

	for (auto & e : entities) {
		auto * bc = e.getComponent<Components::BasicDrawSystemBatch>();
		if (bc != nullptr && bc->isBatched) continue;

		// check for billboard
		auto * billV = e.getComponent<Components::BillboardVarying>();
		if (billV) {
			drawBillboardVarying(e);
			continue;
		}

		// by default, assume there is a texture
		glUniform1ui(useTextureU, GL_TRUE);
		glUniform1ui(isSkeletalU, GL_FALSE);

		// grab and load the model transform
		auto * modelTransform = e.getComponent<Components::ModelTransform>();
		auto * location = e.getComponent<Components::Position3D<> >();
		auto * o3dPtr = e.getComponent<Components::CObject3D>();

		// apply model transform, then translate by Position3D 
		glm::mat4 modelTransformMatrix =
			glm::translate(glm::mat4(1), glm::vec3(location->x, location->y, location->z)) * modelTransform->transform;

		glUniformMatrix4fv(modelTransformU, 1, GL_FALSE, &modelTransformMatrix[0][0]);

		// apply transform to normals
		glm::mat4 ti_model_transform = glm::transpose(glm::inverse(modelTransformMatrix));
		glUniformMatrix4fv(tiModelTransformU, 1, GL_FALSE, &ti_model_transform[0][0]);

		// if its a multimesh, issue call and we're done
		auto * multi = e.getComponent<Components::MultiObject3D>();
		if (multi != NULL) {
			drawMulti(e.getHandle());
			continue;
		}

		// bind texture buffer
		glActiveTexture(GL_TEXTURE0); // set texture slot 0 as active
		glBindTexture(GL_TEXTURE_2D, o3dPtr->texId); // bind cubeTex_ID to active texture
		glUniform1i(textureHW, 0); // send value 0 to fragmentTextureSampler uniform

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
			glUniform1ui(useTextureU, GL_FALSE);
			glEnableVertexAttribArray(3);
			glBindBuffer(GL_ARRAY_BUFFER, col3d->bufId);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
		}

		// check for SkeletalAnimation
//		auto * sa = e.getComponent<Components::SkeletalAnimation>();
		auto * sa = e.getComponent<Components::SkeletalAnimationController>();
		if (sa != nullptr) HandleSkeletal(sa);

		// call draw
		glDrawArrays(o3dPtr->drawMode, 0, o3dPtr->count);

		// disable vertex arrays
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
	}
	auto err = glGetError();
	glClearErrors();

}  //  void Systems::BasicDrawSystem::run(std::vector<ECS::Entity> & entities)


// draw multimesh object
void Systems::BasicDrawSystem::drawMulti(ECS::EntityHdl multiHdl) {
	auto * world = getWorld();
	auto * multi = world->getComponent<Components::MultiObject3D>(multiHdl);
	
	for (unsigned int m = 0; m < multi->objects.size(); m++) {
		auto * o3dPtr = &multi->objects[m];

		// bind texture buffer
		glActiveTexture(GL_TEXTURE0); // set texture slot 0 as active
		glBindTexture(GL_TEXTURE_2D, o3dPtr->texId); // bind cubeTex_ID to active texture
		glUniform1i(textureHW, 0); // send value 0 to fragmentTextureSampler uniform

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
		auto * col3d = world->getComponent<Components::Colored3D>(multiHdl);
		if (col3d != NULL) {
			glUniform1ui(useTextureU, GL_FALSE);
			glEnableVertexAttribArray(3);
			glBindBuffer(GL_ARRAY_BUFFER, col3d->bufId);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
		}

		// check for SkeletalAnimation
		//auto * sa = world->getComponent<Components::SkeletalAnimation>(multiHdl);
		auto * sa = world->getComponent<Components::SkeletalAnimationController>(multiHdl);
		if (sa != nullptr) {
			HandleSkeletal(sa, multi->offsets[m]);
		}

		// call draw
		glDrawArrays(o3dPtr->drawMode, 0, o3dPtr->count);

		// disable vertex arrays
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
	} // for each mesh

} // DrawMulti

