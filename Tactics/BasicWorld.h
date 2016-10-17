#pragma once

#ifndef __TACTICS_BASIC_WORLD_H__
#define __TACTICS_BASIC_WORLD_H__

#include "Entity.h"

#include "WindowManager.h"
#include "KeyInputSystem.h"

namespace Tactics {

	namespace Worlds {

		// simple world with WindowManager and DrawSystem
		class BasicWorld : public Tactics::ECS::RunnableWorld {
		public:

			struct CameraKeyHandler : public virtual Tactics::Systems::KeyInputSystem,
				public virtual Tactics::ECS::EventHandler<Tactics::Events::ScrollEvent> 
			{
				void handle(const Tactics::Events::ScrollEvent & se);
				void handle(const Tactics::Events::KeyRepeat & kre) {
					update(kre.keyCode);
				}
				void handle(const Tactics::Events::KeyDown & kde) {
					update(kde.keyCode);
				}
				void update(int);

				ECS::EntityHdl camera;
				float mult = 0.1f;
			};

			virtual void setup();

			// get the window
			GLFWwindow * getWindow();

			// disable camera key handler
			void unregisterCameraKeyHandler();

		protected:
			
		private:
			WindowManager * windowManager;
			CameraKeyHandler * cameraKeyHandler;
		};

	}

}


#endif
