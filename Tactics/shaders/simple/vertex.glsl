#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 uvPosition;
layout(location = 2) in vec3 normals;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_transform;

out vec2 UV;

void main() {
// raw coordinates
//    gl_Position = vec4(vertexPosition, 1.f);

// model transform
//  gl_Position = model_transform * vec4(vertexPosition, 1.f);

  // full monty
  gl_Position = projection_matrix * view_matrix * model_transform * vec4(vertexPosition, 1);
  //UV = vec2(uvPosition.x, 1.0 - uvPosition.y);
  //UV = vec2(uvPosition.x, uvPosition.y);
  //UV = uvPosition;
  UV = vec2(uvPosition.x, uvPosition.y);
}
