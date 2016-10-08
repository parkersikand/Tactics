#version 330 core

#define MAX_BONES 4

layout(location = 0) in vec3 vxPosition;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 color;

// per vertex bone info
layout(location = 4) in uint[MAX_BONES] boneIds; // gets locations 4,5,6,7
layout(location = 8) in float[MAX_BONES] boneWeights; // 8,9,10,11

// object bones
uniform mat4[32] object_bones;
// current bone transform
uniform mat4[32] bone_transforms;

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

out vec2 UV;

void main() {

    mat4 pvm = projection_matrix * view_matrix * model_transform;

    // we've defined our light in model space, so multiply the normals by the correct model transform matrix
	vec3 transformedNormal = normalize(ti_model_transform * vec4(normal,0)).xyz;
    
	//intensity = dot(lightDir, normal);
	intensity = dot(lightDir, transformedNormal);

	// pass through color & uv
	vsColor = color;
	UV = uv;

	mat4 finalBone = mat4(1.0);

	if (isSkeletal) {
	  finalBone *= (object_bones[boneIds[0]] * bone_transforms[boneIds[0]] * boneWeights[0]);
	  if(boneWeights[1] > 0)
	    finalBone *= (object_bones[boneIds[1]] * boneWeights[1]); 
	  if(boneWeights[2] > 0)
	    finalBone *= (object_bones[boneIds[2]] * boneWeights[2]);
	  if(boneWeights[3] > 0)
	    finalBone *= (object_bones[boneIds[3]] * boneWeights[3]);
	  //vsColor = vec3(0.f,1.f,0.f);
	}

	gl_Position = pvm * finalBone * vec4(vxPosition,1);
}
