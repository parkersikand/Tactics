#version 330 core

uniform uint objectId;

layout(location = 0) out vec4 color;
layout(location = 1) out vec3 outNormal;

in vec3 normal;

void main() {

  if(objectId > uint(0) ) {
    color = vec4(1.0,1.0,1.0,1.0);
  } else {
    color = vec4(0.0,1.0,0.0,1.0);
  }

}
