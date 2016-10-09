
#include "BasicDrawSystem.h"

#include "Program.h"
#include "CameraSystem.h"
#include "SkeletalAnimation.h"

#include <algorithm>


using namespace Tactics;


Systems::BasicDrawSystem::BasicDrawSystem() {
	// load glsl program
	programId = Util::ShaderProgramHelper::LoadShaders("shaders/toon1/vertex.glsl", "shaders/toon1/fragment.glsl");
	//programId = Util::ShaderProgramHelper::LoadShaders("shaders/simple/vertex.glsl", "shaders/simple/fragment.glsl");
	glUseProgram(programId);
	viewU = glGetUniformLocation(programId, "view_matrix");
	projectionU = glGetUniformLocation(programId, "projection_matrix");
	hwLightDir = glGetUniformLocation(programId, "lightDir");
	modelTransformU = glGetUniformLocation(programId, "model_transform");
	tiModelTransformU = glGetUniformLocation(programId, "ti_model_transform");
	useTextureU = glGetUniformLocation(programId, "useTexture");
	textureHW = glGetUniformLocation(programId, "fragmentTextureSampler");
	isSkeletalU = glGetUniformLocation(programId, "isSkeletal");

	// generate batch buffers
	glGenBuffers(1, &vxVBO);
	glGenBuffers(1, &uvVBO);
	glGenBuffers(1, &normVBO);
	glGenBuffers(1, &colorVBO);
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
	// set global uniforms
	glUniform1ui(isSkeletalU, 0);

	// set camera info
	auto * cameraSystem = getWorld()->getGlobalSystem<CameraSystem>();
	auto vm = cameraSystem->getViewMatrix();
	glUniformMatrix4fv(viewU, 1, GL_FALSE, &vm[0][0]);
	auto pm = cameraSystem->getProjectionMatrix();
	glUniformMatrix4fv(projectionU, 1, GL_FALSE, &pm[0][0]);
	glm::vec3 lightDir = glm::normalize(glm::vec3(5.f, 10.f, 5.f));
	glUniform3fv(hwLightDir, 1, &lightDir[0]);
	glActiveTexture(GL_TEXTURE0); // set texture slot 0 as active
	glUniform1ui(useTextureU, 0);

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
			//glm::mat4 ti_model_transform = glm::transpose(glm::inverse(modelTransformMatrix));
			//glUniformMatrix4fv(tiModelTransformU, 1, GL_FALSE, &ti_model_transform[0][0]);
			glUniformMatrix4fv(tiModelTransformU, 1, GL_FALSE, &modelTransformMatrix[0][0]);

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

// draw unbatched objects
void Systems::BasicDrawSystem::draw(std::vector<ECS::Entity> & entities) {
	glUseProgram(programId);

	// set camera info
	auto * cameraSystem = getWorld()->getGlobalSystem<CameraSystem>();
	auto vm = cameraSystem->getViewMatrix();
	glUniformMatrix4fv(viewU, 1, GL_FALSE, &vm[0][0]);

	auto pm = cameraSystem->getProjectionMatrix();
	glUniformMatrix4fv(projectionU, 1, GL_FALSE, &pm[0][0]);

	glm::vec3 lightDir = glm::normalize(glm::vec3(5.f, 10.f, 5.f));
	glUniform3fv(hwLightDir, 1, &lightDir[0]);

	for (auto & e : entities) {
		auto * bc = e.getComponent<Components::BasicDrawSystemBatch>();
		if (bc == nullptr || bc->isBatched) continue;

		glUniform1ui(useTextureU, GL_TRUE);
		glUniform1ui(isSkeletalU, GL_FALSE);

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

		// check for SkeletalAnimation
		auto * sa = e.getComponent<Components::SkeletalAnimation>();
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
				auto bt = sa->bones[i].translation * sa->bones[i].rotation * sa->bones[i].scale /* * sa->armatureRotation */;
				bt = sa->armatureRotation * sa->bones[i].translation;
				bt = glm::rotate(glm::mat4(1.f), 90.f, glm::vec3(1, 0, 0)) * sa->bones[i].translation;
				bt = glm::mat4(1.f);
				//bt = glm::rotate(glm::mat4(), 90.f, glm::vec3(0,0,1.f)) * glm::translate(glm::mat4(), glm::vec3(5.f, 0.f, 0.f));
				bt = glm::translate(glm::mat4(), glm::vec3(0.f, 5.f, 0.f));
				// TODO figure out why translating and rotating 90 degrees about an axis does not work as expected
				glUniformMatrix4fv(boneTransformU, 1, GL_FALSE, &bt[0][0]);
			}

			glBindBuffer(GL_ARRAY_BUFFER, sa->vertexBoneInfoVBO);
			glEnableVertexAttribArray(4);
			glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(Components::SkeletalAnimation::VertexBoneInfo), (void *)0);
			glEnableVertexAttribArray(5);
			glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(Components::SkeletalAnimation::VertexBoneInfo), (void *)1);
			glEnableVertexAttribArray(6);
			glVertexAttribIPointer(6, 1, GL_UNSIGNED_INT, sizeof(Components::SkeletalAnimation::VertexBoneInfo), (void *)2);
			glEnableVertexAttribArray(7);
			glVertexAttribIPointer(7, 1, GL_UNSIGNED_INT, sizeof(Components::SkeletalAnimation::VertexBoneInfo), (void *)3);
//			glVertexAttribIPointer(4, 4, GL_BYTE, sizeof(Components::SkeletalAnimation::VertexBoneInfo), (void *)0);
			glEnableVertexAttribArray(8);
			glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(Components::SkeletalAnimation::VertexBoneInfo), (void *)4);
			glEnableVertexAttribArray(9);
			glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(Components::SkeletalAnimation::VertexBoneInfo), (void *)8);
			glEnableVertexAttribArray(10);
			glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(Components::SkeletalAnimation::VertexBoneInfo), (void *)12);
			glEnableVertexAttribArray(11);
			glVertexAttribPointer(11, 1, GL_FLOAT, GL_FALSE, sizeof(Components::SkeletalAnimation::VertexBoneInfo), (void *)16);
		}

		// call draw
		glDrawArrays(o3dPtr->drawMode, 0, o3dPtr->count);

		// disable vertex arrays
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);
	}

}  //  void Systems::BasicDrawSystem::run(std::vector<ECS::Entity> & entities)
