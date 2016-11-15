#version 330 core

layout(location = 0) in vec3 vxPosition;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 color;

uniform mat4 view_matrix;
uniform mat4 projection_matrix;

uniform vec3 billboardCenter;
uniform vec2 billboardSize;

out vec3 vColor;
out vec2 vUV;

void main() {
  vec3 cameraRightWorld = vec3(view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]);
  vec3 cameraUpWorld = vec3(view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]);  

  vec3 vxWorld = billboardCenter + (cameraRightWorld * vxPosition.x * billboardSize.x) + (cameraUpWorld * vxPosition.y * billboardSize.y);

  vUV = uv;
  vColor = color;

  // we still need to multiply by projection and view.
  gl_Position = projection_matrix * view_matrix * vec4(vxWorld,1.f);
}






