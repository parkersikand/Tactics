#pragma once

#ifndef __TACTICS_COMMON_H__
#define __TACTICS_COMMON_H__

#include <GL/glew.h>

#define PI 3.1415926535

//#define glClearErrors() while(glGetError());
#define glClearErrors() for(auto e = glGetError(); e != GL_NO_ERROR; e = glGetError());

#endif
