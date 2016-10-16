#pragma once

#ifndef __LINE_COLLISION_H__
#define __LINE_COLLISION_H__

#include "Entity.h"
#include "System.h"

#include "Object3D.h"

namespace Tactics {
	namespace LineCollision {

		namespace Components {
			struct LineCollisionSource : public ECS::Component {};
			struct LineCollisionRay : public ECS::Component {
				float rayLength = INFINITY;
				// How many hits to detect
				int numHits = 1;
				glm::vec3 direction;
			};
			// Tag for an object that can be hit by a line
			struct LineCollisionTarget : public ECS::Component {};
		} // Tactics::LineCollision::Components

		namespace Events {
			struct LineCollisionEvent {
				ECS::EntityHdl source;
				ECS::EntityHdl ray;
				ECS::EntityHdl target;
				float depth; // how far away the hit object was
			};
		} // Tactics::LineCollision::Events

		namespace Systems {

			// Manages Sources, Rays, and Targets
			// Fires events when collisions are detected
			class LineCollisionDetector : public ECS::System, 
				public ECS::AutoEventHandler<ECS::Events::EntityDestroyedEvent> {
			public:

				struct Result {
					glm::vec4 color;
					glm::vec3 normal;
					float depth;
				};

				LineCollisionDetector();
				
				// OVERRIDES  ////////////////////////////////////////////////

				// Detect collisions and fire events
				virtual void run(ECS::World &);
				
				// When an entity is destroyed, update lists
				void handle(const ECS::Events::EntityDestroyedEvent & evt);

				//  METHODS  /////////////////////////////////////////////////
				void addRaySource(const ECS::EntityHdl source) {
					source_rays.push_back(source);
				};

				void fireRay(const Tactics::Components::Position3D<> & pos, Components::LineCollisionRay ray);
				
				// examine one ray and one target
				bool fireRay(const Tactics::Components::Position3D<> & pos, Components::LineCollisionRay ray, ECS::EntityHdl hdl, bool fireEvent = true);


				// get color info from linecast
				glm::vec4 color(ECS::EntityHdl, ECS::EntityHdl);

				// get normal info from linecast
				glm::vec3 normal(ECS::EntityHdl, ECS::EntityHdl);
				
				// get depth info from linecast
				float depth(ECS::EntityHdl, ECS::EntityHdl);

				// get full result from cast
				Result castResult(ECS::EntityHdl, ECS::EntityHdl);

			private:

				// glsl program
				GLuint programId;

				// Ray sources
				std::vector<ECS::EntityHdl> source_rays;

				// Grain of buffers
				unsigned int bufferAccuracy = 1000;
				
				void LineCollisionDraw(const Tactics::Components::Position3D<> & pos,
					Components::LineCollisionRay ray,
					ECS::EntityHdl targetHdl);

				// Ray targets
				// TODO figure out how/if to track ray targets. For now, use all of them
				//std::vector<ECS::EntityHdl> targets;

				Result genericCast(ECS::EntityHdl, ECS::EntityHdl, int, void *);

				inline float linearizeDepth(float);

			};

		} // Tactics::LineCollision::Systems

	} // namespace Tactics::LineCollision
} // namespace Tactics

#endif
