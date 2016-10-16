#pragma once

// include file to change main worlds
//#include "AnimatedCubeWorld.h"
//#include "TexturedCubeWorld.h"
#include "WalkOnSurface.h"

// define a class "MainWorld" that extends the desired world functionality
//class MainWorld : public AnimatedCubeWorld {};
//class MainWorld : public TexturedCubeWorld {};
class MainWorld : public Tactics::Worlds::WalkOnSurfaceWorld {};
