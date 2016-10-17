#pragma once

#ifndef __TACTICS_DRAWLOOP_H__
#define __TACTICS_DRAWLOOP_H__

#include "System.h"
#include "TimedFiringSystem.h"
#include "EventedSystem.h"
#include "DrawSystem.h"

namespace Tactics {

	struct DrawEvent : public ECS::Event {};
	/*
	namespace Components {

		// Anything that can be drawn
		struct Drawable : public ECS::Component {};

	} // namespace Components
	*/
	namespace Systems {

		// Runnable system that fires a draw event every 30 ms
		struct DrawEventShooter : public TimedFiringSystem<DrawEvent> {
			DrawEventShooter() : TimedFiringSystem<DrawEvent>(30.0) {};
		};

		// Global system that contains child drawing systems
		class DrawLoop : public virtual EventedSystem<DrawEvent> {
		public:
			
			DrawLoop() {};
			DrawLoop(GLFWwindow * wind);

			void handle(const DrawEvent &);

			void addSystem(BaseAbstractDrawSystem & sys) {
				systems.push_back(&sys);
			}

			void setWindow(GLFWwindow * wind) {
				myWindow = wind;
				glfwGetWindowSize(wind, reinterpret_cast<int*>(&width), reinterpret_cast<int*>(&height));
			}

		private:
			// the window that this system will draw to
			GLFWwindow * myWindow;

			unsigned int width, height;

			std::vector<Systems::BaseAbstractDrawSystem *> systems;

		}; // class DrawLoop

	} // namespace Tactics::Systems

} // namespace Tactics

#endif
