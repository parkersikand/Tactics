#ifdef TESTING


#include "WalkOnSurface.h"
#include "DrawLoop.h"

#include <gtest/gtest.h>


TEST(WalkOnSurface, visualize_level_1) {
	Tactics::Worlds::WalkOnSurfaceWorld world;
	world.setup();
	// issue draw call
	Tactics::DrawEvent de;
	Tactics::ECS::EventDispatcher::postEvent(de);
	SUCCEED();
}


TEST(WalkOnSurface, Step1) {
	Tactics::Worlds::WalkOnSurfaceWorld world;
	world.setup();
	ASSERT_TRUE(world.testStep1());
}




#endif
