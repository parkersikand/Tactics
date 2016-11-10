#pragma once

#ifndef __TACTICS_BASIC_DRAW_H__
#define __TACTICS_BASIC_DRAW_H__

#include <GL/glew.h>
#include "DrawSystem.h"
#include "Object3D.h"
#include "SkeletalAnimation.h"

namespace Tactics {

	namespace Components {

		struct BasicDrawSystemBatch : public ECS::Component {
			bool isBatched = false;
		};

	}

	namespace Systems {

		class BasicDrawSystem : public AbstractDrawSystem<BasicDrawSystem> {
		public:
			BasicDrawSystem();
			void run(std::vector<ECS::Entity> &);

			// update batched VBOs. This is slow, so don't call this very often
			void makeBatches();

			void setClearColor(glm::vec4 col) { clearColor = col; }
			glm::vec4 getClearColor() { return clearColor; }

			// send SkeletalAnimation data to shaders
			//void HandleSkeletal(Components::SkeletalAnimation * sa, unsigned int startLocation = 0);
			void HandleSkeletal(Components::SkeletalAnimationController * sa, unsigned int startLocation = 0);

			struct FogParameters {
				glm::vec4 fogColor = glm::vec4(0.7f,0.7f,0.7f,1.f);
				float fogDensity = 0.04f;
			};

			glm::vec4 getFogColor() { return fogParams.fogColor; }
			void setFogColor(glm::vec4 c) { fogParams.fogColor = c; }
			float getFogDensity() { return fogParams.fogDensity; }
			void setFogDensity(float d) { fogParams.fogDensity = d; }

			void drawMulti(ECS::EntityHdl);

		private:

			// a batch of objects for the basic draw system
			// all objects in a batch use the same texture
			struct Batch {

				Batch();

				// info needed for one object
				struct ObjectInfo {
					ECS::EntityHdl handle;
					unsigned int count;  // number of vertices in object
					unsigned int index;
				};

				std::vector<ObjectInfo> objectInfo;

				unsigned int objectCount = 0;
				unsigned int vertexCount = 0;

				static const unsigned int MAX_VERTEX_COUNT = 65535;

				GLuint drawType =  GL_TRIANGLES;

				GLuint textureId;
				bool useTexture;
				GLuint vxVBO, normVBO, uvVBO, colorVBO;
			};

			GLuint programId;

			// uniforms
			GLuint viewU, projectionU, modelTransformU, tiModelTransformU, useTextureU, textureHW, hwLightDir, viewDirectionU;
			GLuint cameraPosU;
			GLuint isSkeletalU;

			// batched VBOS
			GLuint vxVBO, uvVBO, normVBO, colorVBO;

			// fog uniforms
			GLuint fogColorU, fogDensityU;

			FogParameters fogParams;

			// bone uniforms
			GLuint boneU[64];
			GLuint boneITU[64];

			// clear color
			glm::vec4 clearColor = glm::vec4(0.f, 0.f, 0.4f, 0.f);

			// ordered handles
			std::vector<ECS::EntityHdl> orderedBatchHandles;

			// draw a single unbatched object
			void draw(std::vector<ECS::Entity> &);

			// hold batches
			std::vector<Batch> batches;
		};

	} // namespace Systems

	// Template specialization helper for adding components
	template <>
	inline Components::DrawSystemTag<Systems::BasicDrawSystem> * ECS::World::addComponent(ECS::EntityHdl e) {
		addComponent(e, Components::Position3D<>());
		addComponent(e, Components::CObject3D());
		addComponent(e, Components::ModelTransform());
		//addComponent(e, Components::BasicDrawSystemBatch());
		return addComponent(e, Components::DrawSystemTag<Systems::BasicDrawSystem>());
	}

} // Tactics

#endif
