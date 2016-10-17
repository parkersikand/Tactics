#version 330 core

// Ray Select Fragment Shader

uniform uint objectId;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 outNormal;

in vec3 varyingNormal;

void main() {

  outNormal = vec4(varyingNormal, 1.f);

  if(objectId > uint(0) ) {
    color = vec4(1.0,1.0,1.0,1.0);
  } else {
    color = vec4(0.0,1.0,0.0,1.0);
  }

}
