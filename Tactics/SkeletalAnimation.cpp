
#include "SkeletalAnimation.h"

#include <gtc/quaternion.hpp>

using namespace Tactics;


bool SkeletalAnimation::SkeletalAnimationHelper::LoadVertexBoneInfo(Components::SkeletalAnimation * sa, std::vector<Components::SkeletalAnimation::VertexBoneInfo> & vbi) {

	// create a new VBO
	glDeleteBuffers(1, &sa->vertexBoneInfoVBO);
	glGenBuffers(1, &sa->vertexBoneInfoVBO);
	glBindBuffer(GL_ARRAY_BUFFER, sa->vertexBoneInfoVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Components::SkeletalAnimation::VertexBoneInfo) * vbi.size(), &vbi[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// TODO check for errors
	
	// delete the raw data in the component
	sa->vertexBoneInfo.clear();

	return true;
}


bool SkeletalAnimation::SkeletalAnimationHelper::LoadVertexBoneInfo(Components::SkeletalAnimation * sa) {
	return LoadVertexBoneInfo(sa, sa->vertexBoneInfo);
}


bool SkeletalAnimation::SkeletalAnimationHelper::LoadVertexBoneInfo(
	Components::SkeletalAnimation * sa,
	std::vector<std::vector<unsigned int> > & idx,
	std::vector<std::vector<float> > & weights) 
{
	std::vector<Components::SkeletalAnimation::VertexBoneInfo> data;
	Components::SkeletalAnimation::VertexBoneInfo vbi;
	for (unsigned int i = 0; i < idx.size(); i++) {
		for (unsigned int b = 0; b < MAX_BONES; b++) {
			vbi.boneId[b] = -1;
			vbi.boneWeight[b] = 0.f;
		}
		for (unsigned int b = 0; b < idx[i].size(); b++) {
			assert(b < MAX_BONES);
			vbi.boneId[b] = idx[i][b];
			vbi.boneWeight[b] = weights[i][b];
		}
		data.push_back(vbi);
	}

	return LoadVertexBoneInfo(sa, data);
}

glm::mat4 linearInterpolateMatrices(glm::mat4 a, glm::mat4 b, double percent) {
	glm::mat4 out;

	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			out[i][j] = a[i][j] + percent * (b[i][j] - a[i][j]);
		}
	}

	return out;
}


glm::mat4 InterpolateRotationMatrices(glm::mat4 a, glm::mat4 b, double factor) {
	glm::quat qa = glm::quat_cast(a);
	glm::quat qb = glm::quat_cast(b);
	return glm::mat4_cast(glm::slerp(qa, qb, (float)factor));
}


void SkeletalAnimation::SkeletalAnimationHelper::InterpolateBoneTransforms(Components::SkeletalAnimation * sa, double offset) {
	auto anim = sa->animations[sa->currentAnimation];
	
	// if we are in the middle of an animation
	if (offset > 0 && offset < sa->tick_ms * (anim.keyFrames.size()-1)) {
		unsigned int kf = 0;
		// find first frame
		while ((kf+1)*sa->tick_ms < offset) kf++;
		assert(kf < anim.keyFrames.size()-1);

		double d = offset - sa->tick_ms * kf;
		assert(d < sa->tick_ms);

		// interpolate transforms
		double p = d / sa->tick_ms;
		assert(p >= 0 && p <= 1.0);

		for (auto & bone : sa->bones) {
			auto it = anim.keyFrames.begin();
			for (unsigned int i = 0; i < kf; i++) ++it;
			// bone transform at first frame
			auto t1 = it->second.bones[bone.boneName];
			// bone transform at second frame
			++it;
			auto t2 = it->second.bones[bone.boneName];
		    bone.rotation = InterpolateRotationMatrices(t1.rotation, t2.rotation, p);
			bone.translation = linearInterpolateMatrices(t1.translation, t2.translation, p);
			bone.scale = linearInterpolateMatrices(t1.scale, t2.scale, p);
		} // for each bone

	}
	else if (offset < 0.001) { // zero, first keyframe
		for (auto & bone : sa->bones) {
			bone.rotation = anim.keyFrames.begin()->second.bones[bone.boneName].rotation;
			bone.translation = anim.keyFrames.begin()->second.bones[bone.boneName].translation;
			bone.scale = anim.keyFrames.begin()->second.bones[bone.boneName].scale;
		}
	}
	else { // end of animation, last keyframe
		for (auto & bone : sa->bones) {
			bone.rotation = anim.keyFrames.rbegin()->second.bones[bone.boneName].rotation;
			bone.translation = anim.keyFrames.rbegin()->second.bones[bone.boneName].translation;
			bone.scale = anim.keyFrames.rbegin()->second.bones[bone.boneName].scale;
		}
	}
}  //  InterpolateBoneTransforms
