#pragma once

#ifndef __TACTICS_GUIDE_LINES_H__
#define __TACTICS_GUIDE_LINES_H__

#include "Entity.h"

namespace Tactics {
	namespace Entities {
		struct GuideLines : Tactics::ECS::Entity {
			GuideLines(Tactics::ECS::World & w);
			bool showX = true, showY = true, showZ = true;
		};
	}
}

#endif
