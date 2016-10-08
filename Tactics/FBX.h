#pragma once

#ifndef __TACTICS_FBX_H__
#define __TACTICS_FBX_H__

#include <GL/glew.h>
#include "Object3D.h"
#include "SkeletalAnimation.h"

#include <vector>
#include <iostream>

namespace Tactics {

	namespace Util {

		namespace FBX {

			template <typename T>
			std::vector<T> & readMany(std::istream & in, std::vector<T> & v) {
				T tmp;
				while (in >> tmp) {
					v.push_back(tmp);
					if (in.peek() != ',') break;
					in.get(); // discard ','
				}
				return v;
			}

			bool LoadMesh(const char * filename, Tactics::Components::CObject3D *, bool flipUVs = false, const char * meshname = NULL);

			SkeletalAnimation::Bone LoadBoneHierarchy(const char * filename, const char * meshname = NULL);
			
			bool LoadSkeletalAnimation(const char * filename, Tactics::Components::SkeletalAnimation *, Tactics::Components::CObject3D *, const char * meshname = NULL);

		}// namespace Tactics::Util::FBX
	} // namespace Tactics::Util
} // namespace Tactics




#endif
