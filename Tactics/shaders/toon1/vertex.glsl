#version 330 core

// Cartoon Shader V1

#define MAX_BONES 4

layout(location = 0) in vec3 vxPosition;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 color;

// per vertex bone info
layout(location = 4) in ivec4 boneIds;
layout(location = 5) in vec4 boneWeights;

// bones with transformation
uniform mat4[64] bones;
uniform mat3[64] bonesIT;

// Material info
uniform vec3 diffuse;

// normalized light direction
uniform vec3 lightDir;

// computed intensity of color
out float intensity;

out vec3 vsColor;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_transform;
uniform mat4 ti_model_transform;

// skeletal flag
uniform bool isSkeletal;

// pass through UV
out vec2 UV;

// pass through normal
out vec3 varyingNormal;

// pass through position
out vec4 VXpassthrough;
// pass through eyespace
out vec4 eyeSpaceVX;

void main() {
    // pass through color & uv
	vsColor = color;
	UV = uv;
	varyingNormal = normal;
	VXpassthrough = vec4(vxPosition,1.f);

	mat4 modelView = view_matrix * model_transform;

    mat4 pvm = projection_matrix * view_matrix * model_transform;

    // we've defined our light in model space, so multiply the normals by the correct model transform matrix
	vec3 transformedNormal = normalize(ti_model_transform * vec4(normal,0)).xyz;
    
	

	mat4 finalBone = mat4(0.0);
	mat3 finalBoneIT = mat3(0.0);

	if (isSkeletal) {
	  //vsColor = vec3(0.f,1.f,0.f);

	  finalBone = bones[boneIds[0]] * boneWeights[0];
	  finalBone += bones[boneIds[1]] * boneWeights[1];
	  finalBone += bones[boneIds[2]] * boneWeights[2];
	  finalBone += bones[boneIds[3]] * boneWeights[3];

	  finalBoneIT = bonesIT[boneIds[0]] * boneWeights[0];
	  finalBoneIT += bonesIT[boneIds[1]] * boneWeights[1];
	  finalBoneIT += bonesIT[boneIds[2]] * boneWeights[2];
	  finalBoneIT += bonesIT[boneIds[3]] * boneWeights[3];

	} else {
	  finalBone = mat4(1.f);
	  finalBoneIT = mat3(1.f);
	}

	//intensity;
	intensity = dot(lightDir, mat3(ti_model_transform) * finalBoneIT * normal);

	gl_Position = pvm * finalBone * vec4(vxPosition,1);
	eyeSpaceVX = modelView * finalBone * vec4(vxPosition,1);

	// debug
    //  gl_Position = pvm * vec4(vxPosition, 1);
}
