
#include "SkeletalAnimation.h"

#include "assimp_loader.h"

#include <ogldev_math_3d.h>

#include <gtc/quaternion.hpp>
#include <gtc/matrix_transform.hpp>

#include "Util.h"

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

/*
glm::mat4 aiMatrix4x4_mat4(aiMatrix4x4 input) {
	glm::mat4 output;
	for (unsigned int i = 0; i < 4; i++)
		for (unsigned int j = 0; j < 4; j++)
			output[i][j] = input[i][j];
	return output;
}


glm::mat4 aiMatrix3x3_mat4(aiMatrix3x3 input) {
	glm::mat4 output(1.f); // initialize identity matrix
	for (unsigned int i = 0; i < 3; i++)
		for (unsigned int j = 0; j < 3; j++)
			output[i][j] = input[i][j];
	return output;
}
*/

unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


// TODO this is slow
/*
const aiNodeAnim* Tactics::Components::SkeletalAnimation::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}
*/
const aiNodeAnim * Tactics::Components::SkeletalAnimation::FindNodeAnim(const std::string & NodeName) {
	return named_animations[NodeName];
}

void Tactics::Components::SkeletalAnimation::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, glm::mat4 parentTransform)
{
	std::string NodeName(pNode->mName.data);

//	const aiAnimation* pAnimation = pscene->mAnimations[0];

	//Matrix4f NodeTransformation(pNode->mTransformation);
	glm::mat4 NodeTransformation = aiMatrix4x4_mat4(pNode->mTransformation);

//	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
	const aiNodeAnim * pNodeAnim = FindNodeAnim(NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		glm::mat4 ScalingM = glm::scale(glm::mat4(1.f), glm::vec3(Scaling.x, Scaling.y, Scaling.z));

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		//glm::mat4 RotationM = aiMatrix3x3_mat4(RotationQ.GetMatrix());
		glm::mat4 RotationM = aiMatrix3x3_mat4(RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		glm::mat4 TranslationM = glm::transpose(glm::translate(glm::mat4(1.f), glm::vec3(Translation.x, Translation.y, Translation.z)));

		// Combine the above transformations
		NodeTransformation = ScalingM * RotationM * TranslationM;  // transpose of ogldev matrix
	}

	glm::mat4 GlobalTransformation = NodeTransformation * parentTransform;

	if (name_id.find(NodeName) != name_id.end()) {
		unsigned int BoneIndex = name_id[NodeName];
//		bone_transforms[BoneIndex] = globalInverseTransform * GlobalTransformation * bones[BoneIndex].boneSpace;
		bone_transforms[BoneIndex] = bones[BoneIndex].boneSpace * GlobalTransformation * globalInverseTransform;
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}


glm::mat4 mmul(glm::mat4 a, glm::mat4 b) {
	glm::mat4 out;
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			out[r][c] = 0.f;
			for (int i = 0; i < 4; i++) {
				out[r][c] += a[r][i] * b[i][c];
			}
		}
	}
	return out;
}


void Tactics::Components::SkeletalAnimation::ReadNodeHeirarchyDebug(float AnimationTime, const aiNode* pNode, Matrix4f ParentTransform, std::vector<glm::mat4> & trace)
{
	std::string NodeName(pNode->mName.data);

//	const aiAnimation* pAnimation = pscene->mAnimations[0];

	Matrix4f NodeTransformationM4(pNode->mTransformation);
	glm::mat4 NodeTransformation = aiMatrix4x4_mat4(pNode->mTransformation);
	trace.push_back(NodeTransformation);
	if (!mateq(NodeTransformationM4, NodeTransformation)) {
		int stop = 0;
		throw std::exception("NodeTransformInit");
	}
	assert(mateq(NodeTransformationM4, NodeTransformation));

//	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
	const aiNodeAnim * pNodeAnim = FindNodeAnim(NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		Matrix4f ScalingM4;
		ScalingM4.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
		//glm::mat4 ScalingM = glm::scale(glm::mat4(1.f), glm::vec3(Scaling.x, Scaling.y, Scaling.z));
		glm::mat4 ScalingM = Matrix4f2mat4(ScalingM4);
		trace.push_back(ScalingM);
		if (!mateq(ScalingM4, ScalingM)) {
			int stop = 0;
			throw std::exception("Scaling");
		}
		assert(mateq(ScalingM4, ScalingM));

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		Matrix4f RotationM4 = Matrix4f(RotationQ.GetMatrix());
		glm::mat4 RotationM = aiMatrix3x3_mat4(RotationQ.GetMatrix());
		trace.push_back(RotationM);
		if (!mateq(RotationM4, RotationM)) {
			int stop = 0;
			throw std::exception("Rotation");
		}
		assert(mateq(RotationM4, RotationM));
		
		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		Matrix4f TranslationM4;
		TranslationM4.InitTranslationTransform(Translation.x, Translation.y, Translation.z);
		glm::mat4 TranslationM = glm::transpose(glm::translate(glm::mat4(1.f), glm::vec3(Translation.x, Translation.y, Translation.z)));
		trace.push_back(TranslationM);
		if (!mateq(TranslationM4, TranslationM)) {
			int stop = 0;
			throw std::exception("Translation");
		}
		assert(mateq(TranslationM4, TranslationM));
		

		// Combine the above transformations
		NodeTransformationM4 = TranslationM4 * RotationM4 * ScalingM4;
		NodeTransformation = ScalingM * RotationM * TranslationM;
//		NodeTransformation = mmul(TranslationM, mmul(RotationM, ScalingM));
		trace.push_back(NodeTransformation);
		if (!mateq(NodeTransformationM4, NodeTransformation)) {
			int stop = 0;
			throw std::exception("Final NodeTransform");
		}
		assert(mateq(NodeTransformationM4, NodeTransformation));
	}

	Matrix4f GlobalTransformationM4 = ParentTransform * NodeTransformationM4;
	glm::mat4 GlobalTransformation = NodeTransformation * Matrix4f2mat4(ParentTransform);
	//GlobalTransformation = mmul(Matrix4f2mat4(ParentTransform), NodeTransformation); // works
	trace.push_back(GlobalTransformation);
	if (!mateq(GlobalTransformationM4, GlobalTransformation)) {
		int stop = 0;
		throw std::exception("GlobalTransformation");
	}
	assert(mateq(GlobalTransformationM4, GlobalTransformation));

	if (name_id.find(NodeName) != name_id.end()) {
		unsigned int BoneIndex = name_id[NodeName];
		bone_transforms[BoneIndex] = bones[BoneIndex].boneSpace * GlobalTransformation * globalInverseTransform;
//		bone_transforms[BoneIndex] = mmul(globalInverseTransform, mmul(GlobalTransformation, bones[BoneIndex].boneSpace)); // works
		debug_transforms[BoneIndex] = globalInverseTransformM4 * GlobalTransformationM4 * debug_bones[BoneIndex];
		trace.push_back(bone_transforms[BoneIndex]);
		if (!mateq(debug_transforms[BoneIndex], bone_transforms[BoneIndex])) {
			int stop = 0;
			throw std::exception("bone transform");
		}
		assert(mateq(debug_transforms[BoneIndex], bone_transforms[BoneIndex]));
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchyDebug(AnimationTime, pNode->mChildren[i], GlobalTransformationM4, trace);
	}
}

// compute final bone transforms
//Matrix4f Mesh::BoneTransform(float TimeInSeconds, vector<Matrix4f>& Transforms)
void Components::SkeletalAnimation::BoneTransforms(float TimeInSeconds)
{
	float TicksPerSecond = (float)pscene->mAnimations[0]->mTicksPerSecond != 0.f ?
		pscene->mAnimations[0]->mTicksPerSecond : 25.0f;
	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, pscene->mAnimations[0]->mDuration);

	bone_transforms.resize(bones.size());
	ReadNodeHeirarchy(AnimationTime, pscene->mRootNode, glm::mat4(1.f));
} // BoneTransforms


  // compute final bone transforms
  //Matrix4f Mesh::BoneTransform(float TimeInSeconds, vector<Matrix4f>& Transforms)
/*
glm::mat4 Components::SkeletalAnimation::BoneTransformsDebug(float TimeInSeconds)
{

	float TicksPerSecond = (float)pscene->mAnimations[0]->mTicksPerSecond != 0.f ?
		pscene->mAnimations[0]->mTicksPerSecond : 25.0f;

	float TimeInTicks = TimeInSeconds * TicksPerSecond;

	float AnimationTime = fmod(TimeInTicks, pscene->mAnimations[0]->mDuration);

	bone_transforms.resize(bones.size());
	//return ReadNodeHeirarchyDebug(AnimationTime, pscene->mRootNode, glm::mat4(1.f));
	

} // BoneTransforms
*/
