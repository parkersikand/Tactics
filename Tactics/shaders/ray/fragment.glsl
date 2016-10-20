#version 330 core

// Ray Select Fragment Shader

uniform uint objectId;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outVertex;

in vec3 varyingNormal;

in vec4 VXpassthrough;

// convert a float from [-1,1] to [0,1]
float shiftVal(float val) {
  return (val + 1) / 2;
}

vec4 shiftVec4(vec4 val) {
  vec4 outgoing;
  outgoing.x = shiftVal(val.x);
  outgoing.y = shiftVal(val.y);
  outgoing.z = shiftVal(val.z);
  outgoing.w = shiftVal(val.w);
  return outgoing;
}

void main() {

  outNormal = vec4(varyingNormal, 1.f);
//  outNormal = shiftVec4(vec4(varyingNormal, 1.f));

  outVertex = VXpassthrough;

  if(objectId > uint(0) ) {
    color = vec4(1.0,1.0,1.0,1.0);
  } else {
    color = vec4(0.0,1.0,0.0,1.0);
  }

}
