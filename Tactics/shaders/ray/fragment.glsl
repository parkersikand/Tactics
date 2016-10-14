#version 330 core

uniform ivec4 objectId;

out vec4 color;

void main() {
  color = objectId;
}
