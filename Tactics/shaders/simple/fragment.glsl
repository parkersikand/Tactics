#version 330 core

out vec3 color;

uniform sampler2D fragmentTextureSampler;

in vec2 UV;

void main() {
  // default error color
  color = vec3(0.0,1.0,0.0);

  //color = texture( fragmentTextureSampler, UV ).rgb;
}
