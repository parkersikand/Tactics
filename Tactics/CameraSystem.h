#pragma once

#ifndef __TACTICS_CAMERA_SYSTEM_H__
#define __TACTICS_CAMERA_SYSTEM_H__

#include "System.h"
#include "Event.h"

#include "EventedSystem.h"
#include "Object3D.h"
#include "DrawSystem.h"

#include "glm.hpp"
#include "GL/glew.h"

namespace Tactics {

	namespace Events {

		struct CameraChangedEvent : public ECS::Event {
			ECS::EntityHdl newCamera;
		};

	} // namespace Events
	

	namespace Components {

		struct CameraComponent : public ECS::Component {
			glm::vec3 lookAt = glm::vec3(0,0,0);
			enum ProjectionType { PERSPECTIVE , ISOMETRIC };
			ProjectionType projectionType = PERSPECTIVE;
		};

	} // namespace Components


	// Manages the camera
	class CameraSystem : public EventedSystem<Events::CameraChangedEvent> {
	public:

		void handle(const Events::CameraChangedEvent & evt) {
			currentCamera = evt.newCamera;
		}

		glm::mat4 getProjectionMatrix() {
			ECS::Entity cameraE = getWorld()->entityFromHandle(currentCamera);
			auto * cameraInfo = cameraE.getComponent<Components::CameraComponent>();
			if (cameraInfo->projectionType == cameraInfo->PERSPECTIVE) {
				//return glm::perspective(glm::radians(45.0f), 4.f / 3.f, zNear, zFar);
				return glm::perspective(glm::radians(45.0f), 4.f / 3.f, 0.01f, 1000.f);
			}
			//return glm::ortho(-10.f, 10.f, -10.f, 10.f, zNear, zFar);
			return glm::ortho(-10.f, 10.f, -10.f, 10.f, -10.f, 10.f);
		}

		glm::mat4 getViewMatrix() {
			ECS::Entity cameraE = getWorld()->entityFromHandle(currentCamera);
			auto * location = cameraE.getComponent<Components::Position3D<> >();
			auto * cameraInfo = cameraE.getComponent<Components::CameraComponent>();
			//return glm::lookAt(glm::vec3(location->x, location->y, location->z), cameraInfo->lookAt, glm::vec3(0, 1, 0));
			return glm::lookAt(glm::vec3(location->x, location->y, location->z), cameraInfo->lookAt, up);
		}

		glm::vec3 getViewDirection() {
			ECS::Entity cameraE = getWorld()->entityFromHandle(currentCamera);
			auto * location = cameraE.getComponent<Components::Position3D<> >();
			auto * cameraInfo = cameraE.getComponent<Components::CameraComponent>();
			return glm::vec3(location->x, location->y, location->z) - cameraInfo->lookAt;
		}

		glm::vec3 getCameraPos() {
			ECS::Entity cameraE = getWorld()->entityFromHandle(currentCamera);
			auto * location = cameraE.getComponent<Components::Position3D<> >();
			return glm::vec3(location->x, location->y, location->z);
		}

		float getZNear() { return zNear; }
		void setZNear(float f) { zNear = f; }
		float getZFar() { return zFar; }
		void setZFar(float f) { zFar = f; }

		glm::vec3 getUp() { return up; }
		void setUp(glm::vec3 u) { up = u; }

	private:
		ECS::EntityHdl currentCamera;
		
		float zNear = 0.1f;
		float zFar = 1000.f;

		glm::vec3 up = glm::vec3(0, 1, 0);
	};

} // namespace Tactics


#endif
