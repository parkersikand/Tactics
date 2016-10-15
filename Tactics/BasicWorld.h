#pragma once

#ifndef __TACTICS_BASIC_WORLD_H__
#define __TACTICS_BASIC_WORLD_H__

#include "Entity.h"

#include "WindowManager.h"

namespace Tactics {

	namespace Worlds {

		// simple world with WindowManager and DrawSystem
		class BasicWorld : public virtual Tactics::ECS::RunnableWorld {
		public:
			virtual void setup();
			// get the window
			GLFWwindow * getWindow();
		protected:
			
		private:
			WindowManager * windowManager;
		};

	}

}


#endif
