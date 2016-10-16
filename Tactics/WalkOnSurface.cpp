

#include "WalkOnSurface.h"

#include "KeyInputSystem.h"
#include "LineCollision.h"


using namespace Tactics;


class InputHandler : public Tactics::Systems::KeyInputSystem {

};


void Worlds::WalkOnSurfaceWorld::setup() {
	// unregister default input system

}

void
