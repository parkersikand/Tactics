#pragma once

#ifndef __TACTICS_KEY_INPUT_SYSTEM_H__
#define __TACTICS_KEY_INPUT_SYSTEM_H__

#include "System.h"
#include "Event.h"

#include "WindowManager.h"

namespace Tactics {

	namespace Systems {

		// A system to read key input and fire events accordingly
		// really just a facade to setup GLFW key callback
		class KeyInputSystem : public ECS::System, 
			public virtual ECS::EventHandler<Events::KeyDown>, 
			public virtual ECS::EventHandler<Events::KeyUp>,
			public virtual ECS::EventHandler<Events::KeyRepeat> {
		public:
			KeyInputSystem() {};
			//KeyInputSystem(double mt) : memoryTimeMS(mt) {};

			virtual void handle(const Events::KeyDown &) {};
			virtual void handle(const Events::KeyUp &) {};
			virtual void handle(const Events::KeyRepeat &) {};

		private:
			//static KeyInputSystem * instance;
			// The last time we updated the key map
			//double lastTime;

			// how long to remember pressed key before firing another event
			//double memoryTimeMS = 10;

			//std::map<unsigned char, bool> keyMap;

		}; // class KeyInputSystem

	} // namespace Systems

	

} // namespace Tactics




#endif
