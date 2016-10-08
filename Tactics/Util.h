#pragma once

#ifndef __TACTICS_UTIL_H__
#define __TACTICS_UTIL_H__

#include "GLFW/glfw3.h"

#include "Object3D.h"

#include <vector>
#include <iostream>

double avg(std::vector<double> v);

char * readFile(const char * filename);

void printLog(GLuint obj);

// make a unit cube
void make_cube(Tactics::Components::CObject3D *);

// read a quoted string from an input stream
std::string & quotedString(std::istream &, std::string &);


#endif
