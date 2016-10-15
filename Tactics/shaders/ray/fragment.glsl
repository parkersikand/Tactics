#version 330 core

//uniform ivec4 objectId;

uniform uint objectId;

layout(location = 0) out vec4 color;

vec4 ivec2vec4(in ivec4 val) {
  vec4 newval;
  newval.x = val.x / 255;
  newval.y = val.y / 255;
  newval.z = val.z / 255;
  newval.w = val.w / 255;
  return newval;
}

void main() {
//  color = objectId;
//  color = vec4(0.0,1.0,0.0,0.0);
//  color = ivec2vec4(objectId);
  /*
  if(objectId.x == 255) {
    color = vec4(1.0,0.0,0.0,0.0);
  } 
  else if (objectId.x == 0) {
    color = vec4(1.0,1.0,0,0.0);
  } 
  else {
    color = vec4(0.0,1.0,0.0,0.0);
  }
  */

  if(objectId > uint(0) ) {
    color = vec4(1.0,1.0,1.0,1.0);
  } else {
    color = vec4(0.0,1.0,0.0,1.0);
  }

//  color = vec4(0.0,1.0,0.0,1.0);
}
