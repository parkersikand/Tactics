#pragma once

#ifndef __TACTICS_WALK_ON_SURFACE_H__
#define __TACTICS_WALK_ON_SURFACE_H__


#include "Entity.h"
#include "BasicWorld.h"


namespace Tactics {

	namespace Worlds {

		struct WalkOnSurfaceWorld : public Tactics::Worlds::BasicWorld {
			virtual void setup();
		};

	}

}


#endif
