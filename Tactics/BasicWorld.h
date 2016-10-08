#pragma once

#ifndef __TACTICS_BASIC_WORLD_H__
#define __TACTICS_BASIC_WORLD_H__

#include "Entity.h"

namespace Tactics {

	namespace Worlds {

		// simple world with WindowManager and DrawSystem
		class BasicWorld : public virtual Tactics::ECS::RunnableWorld {
		public:
			void setup();
		private:
		};

	}

}


#endif
