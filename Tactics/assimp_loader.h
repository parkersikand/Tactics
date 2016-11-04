#pragma once

#ifndef __ASSIMP_LOADER_H__
#define __ASSIMP_LOADER_H__


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Object3D.h"
#include "SkeletalAnimation.h"

bool LoadMesh(Tactics::Components::CObject3D * obj, const char * filename);

glm::mat4 aiMatrix4x4_mat4(aiMatrix4x4);
glm::mat4 aiMatrix3x3_mat4(aiMatrix3x3);

bool LoadSkeletal(Tactics::Components::SkeletalAnimation * skeletalPtr, std::vector<unsigned int> & indexInfo, const char * filename);

bool LoadMultiMesh(Tactics::Components::MultiObject3D * multiPtr, const char * filename);

#endif
