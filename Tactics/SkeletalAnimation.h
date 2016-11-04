#pragma once

#ifndef __TACTICS_SKELETAL_ANIMATION_H__
#define __TACTICS_SKELETAL_ANIMATION_H__

#define MAX_BONES 4

#include <GL/glew.h>

#include "Component.h"
#include "glm.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <ogldev_math_3d.h>

namespace Tactics {

	namespace SkeletalAnimation {
		// Contains static bone information
		struct Bone {
			Bone * parent = NULL;
			unsigned int boneId;
			std::string boneName;
			glm::mat4 boneSpace; // matrix that transforms a vertex from Model Space to Bone Space
			// transforms
			glm::mat4 scale;
			glm::mat4 rotation;
			glm::mat4 translation;
			std::vector<Bone> children;
		};
	} // namespace Tactics::SkeletalAnimation
	
	namespace Components {

		// Component that contains all info for skeletal animation for an object
		struct SkeletalAnimation : public ECS::Component {
			struct Animation;
			struct VertexBoneInfo;

			std::map<std::string, Animation> animations;
			std::string currentAnimation = "default";
			std::vector<VertexBoneInfo> vertexBoneInfo;
			GLuint vertexBoneInfoVBO = -1;

			std::map<std::string, std::vector<std::string>> hierarchy;
			std::map<std::string, unsigned int> name_id; // map of names to ids
			std::vector<Tactics::SkeletalAnimation::Bone> bones; // static bone info
			std::vector<glm::mat4> bone_transforms; // final transformations for each bone
			glm::mat4 globalInverseTransform;

			//glm::mat4 armatureRotation; // local rotation of armature object. can be used to correct axes
			const aiScene * pscene;

			// store reference to importer to manage scope
			Assimp::Importer importer;

			int tick_ms = 30; // duration of one frame
			double anim_start = 0; // starting time of current animation

			void ReadNodeHeirarchy(float time, const aiNode * node, glm::mat4 parentTransform);
			void ReadNodeHeirarchyDebug(float time, const aiNode * node, Matrix4f ParentTransform, std::vector<glm::mat4> &);

			// debugging values
			std::vector<Matrix4f> debug_bones;
			std::vector<Matrix4f> debug_transforms;
			Matrix4f globalInverseTransformM4;

			// compute final bone transforms
			void BoneTransforms(float time);
			glm::mat4 BoneTransformsDebug(float time);

			// map of node names to animations
			aiAnimation * defaultAnimation;
			std::map<std::string, aiNodeAnim*> named_animations;
			const aiNodeAnim * FindNodeAnim(const std::string &);

		private:

		}; // SkeletalAnimation

		// A single animation
		struct SkeletalAnimation::Animation {
			struct KeyFrame;

			std::string name; // name of this animation
			std::map<float, KeyFrame> keyFrames;

		}; // SkeletalAnimation::Animation

		// Bone weights for a single vertex
		struct SkeletalAnimation::VertexBoneInfo {
			unsigned char boneId[MAX_BONES] = { 0 };
			float boneWeight[MAX_BONES] = { 0.f };
		};

		struct SkeletalAnimation::Animation::KeyFrame {
			SkeletalAnimation::Animation * animation; // the animation this object belongs to

			float timeOffset; // MS from animation start
			int frame;

			// list of bone transforms at this frame
			std::map<std::string, Tactics::SkeletalAnimation::Bone> bones;

		}; // SkeletalAnimation::Animation::KeyFrame

	} // namespace Tactics::Components

	namespace SkeletalAnimation {

		// helper class for loading Skeletal Animations
		class SkeletalAnimationHelper {
		public:

			// load vertex bone info into a VBO
			static bool LoadVertexBoneInfo(Components::SkeletalAnimation *);
			static bool LoadVertexBoneInfo(Components::SkeletalAnimation *, std::vector<Components::SkeletalAnimation::VertexBoneInfo> &);
			static bool LoadVertexBoneInfo(Components::SkeletalAnimation *, std::vector<std::vector<unsigned int> > &, std::vector<std::vector<float> > &);
			
			static void InterpolateBoneTransforms(Components::SkeletalAnimation *, double offset);

		}; // class SkeletalAnimationHelper

	} // namespace Tactics::SkeletalAnimation

} // namespace Tactics


#endif
