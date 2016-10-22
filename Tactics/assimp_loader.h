#pragma once

#ifndef __ASSIMP_LOADER_H__
#define __ASSIMP_LOADER_H__


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Object3D.h"

bool LoadMesh(Tactics::Components::CObject3D * obj, const char * filename);

#endif
