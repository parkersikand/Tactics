#version 330 core

// Ray Select Shader

#define MAX_BONES 4

layout(location = 0) in vec3 vxPosition;
layout(location = 1) in vec3 vxNormal;
layout(location = 3) in vec3 vxColor;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_transform;



void main() {

    mat4 pvm = projection_matrix * view_matrix * model_transform;

	gl_Position = pvm * vec4(vxPosition,1);
}
