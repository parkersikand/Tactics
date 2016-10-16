#ifdef TESTING


#include "WalkOnSurface.h"

#include <gtest/gtest.h>


TEST(WalkOnSurface, Step1) {
	Tactics::Worlds::WalkOnSurfaceWorld world;
	world.setup();
	ASSERT_TRUE(world.testStep1());
}




#endif
