#pragma once

#ifndef __TACTICS_UTIL_H__
#define __TACTICS_UTIL_H__

#include <GL/glew.h>
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

// compute angle between line and plane
float linePlaneAngleRad(glm::vec3 line, glm::vec3 plane);

// compute angle between line and normal of plane
float lineNormalAngleRad(glm::vec3 line, glm::vec3 normal);

#endif
