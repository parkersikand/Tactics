#pragma once

#ifndef __TACTICS_BASIC_DRAW_H__
#define __TACTICS_BASIC_DRAW_H__

#include <GL/glew.h>
#include "DrawSystem.h"
#include "Object3D.h"

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
			GLuint viewU, projectionU, modelTransformU, tiModelTransformU, useTextureU, textureHW, hwLightDir;
			GLuint isSkeletalU;

			// batched VBOS
			GLuint vxVBO, uvVBO, normVBO, colorVBO;

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
		addComponent(e, Components::BasicDrawSystemBatch());
		return addComponent(e, Components::DrawSystemTag<Systems::BasicDrawSystem>());
	}

} // Tactics

#endif
