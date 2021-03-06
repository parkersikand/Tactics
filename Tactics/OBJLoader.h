

#ifndef OBJ_LOADER
#define OBJ_LOADER

#include <vector>

#include <glm.hpp>

#include "Object3D.h"

/**
  Load a file in OBJ format
  Parameters:
    const char * filename,
	vector<glm::vec3> & vertices,
	vector<glm::vec2> & texCoords,
	vector<glm::vec3> & normals
*/
bool LoadObj(const char *, std::vector<glm::vec3> &,
	std::vector<glm::vec2> &,
	std::vector<glm::vec3> &, bool flipUVs = false);

bool LoadObj(Tactics::Components::CObject3D * object3Dpointer, const char * filename);


#endif


