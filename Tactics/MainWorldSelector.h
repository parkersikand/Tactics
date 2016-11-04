#pragma once

// include file to change main worlds




// define a class "MainWorld" that extends the desired world functionality

//#include "AnimatedCubeWorld.h"
//class MainWorld : public AnimatedCubeWorld {};

//#include "TexturedCubeWorld.h"
//class MainWorld : public TexturedCubeWorld {};

//#include "WalkOnSurface.h"
//class MainWorld : public Tactics::Worlds::WalkOnSurfaceWorld {};

//#include "RedCubeWorld.h"
//class MainWorld : public RedCubeWorld {};

//#include "SpinningSuzanne.h"
//class MainWorld : public Tactics::Worlds::SpinningSuzanneWorld {};

#include "BobLamp.h"
class MainWorld : public BobLampWorld {};
