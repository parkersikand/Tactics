#pragma once

#ifndef __TACTICS_UTIL_H__
#define __TACTICS_UTIL_H__

#include <GL/glew.h>
#include "GLFW/glfw3.h"

#include "Object3D.h"

#include <ogldev_math_3d.h>

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

// compare equality of floats
bool eqf(float, float);

// print Matrix4f
ostream & operator<<(ostream & out, const Matrix4f & m);

// convert Matrix4f to glm::mat4
glm::mat4 Matrix4f2mat4(Matrix4f mat);

// check equivalence of Matrix4f and glm::mat4
bool mateq(Matrix4f, glm::mat4);

// get glfw time as float
float glfwGetTimef();

// return a vector of square coordinates
std::vector<glm::vec3> make_square_tris(float size = 0.5f);

#endif
