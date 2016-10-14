#version 330 core

// Ray Select Shader

#define MAX_BONES 4

layout(location = 0) in vec3 vxPosition;

// per vertex bone info
layout(location = 4) in uint[MAX_BONES] boneIds; // gets locations 4,5,6,7
layout(location = 8) in float[MAX_BONES] boneWeights; // 8,9,10,11

// object bones
uniform mat4[32] object_bones;
// current bone transform
uniform mat4[32] bone_transforms;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_transform;

// skeletal flag
uniform bool isSkeletal;

void main() {

    mat4 pvm = projection_matrix * view_matrix * model_transform;

	//intensity = dot(lightDir, normal);
	intensity = dot(lightDir, transformedNormal);

	mat4 finalBone = mat4(1.0);

	if (isSkeletal) {
	  finalBone *= (object_bones[boneIds[0]] * bone_transforms[boneIds[0]] * boneWeights[0]);
	  if(boneWeights[1] > 0)
	    finalBone *= (object_bones[boneIds[1]] * boneWeights[1]); 
	  if(boneWeights[2] > 0)
	    finalBone *= (object_bones[boneIds[2]] * boneWeights[2]);
	  if(boneWeights[3] > 0)
	    finalBone *= (object_bones[boneIds[3]] * boneWeights[3]);
	}

	gl_Position = pvm * finalBone * vec4(vxPosition,1);
}
