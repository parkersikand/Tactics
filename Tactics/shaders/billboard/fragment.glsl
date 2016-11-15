#version 330 core

// Material info
in vec3 vColor;

// UVs
in vec2 vUV;

out vec4 color;

// texture sampler
uniform sampler2D fragmentTextureSampler;

// use texture or color
uniform bool useTexture;

void main() {
	if(useTexture) {
	  vec3 color3 = texture( fragmentTextureSampler, vUV ).rgb;
	  color = vec4(color3,1.f);
	} else {
	  color = vec4(vColor,1.f);
	}
}
