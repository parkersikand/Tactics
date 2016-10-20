#version 330 core

// raw normal
in vec3 varyingNormal;

// computed intensity
in float intensity;

// Material info
in vec3 vsColor;

// UVs
in vec2 UV;

out vec4 color;

// texture sampler
uniform sampler2D fragmentTextureSampler;

// use texture or color
uniform bool useTexture;

vec3 rgb2hsl( in vec3 c ){
    float h = 0.0;
	float s = 0.0;
	float l = 0.0;
	float r = c.r;
	float g = c.g;
	float b = c.b;
	float cMin = min( r, min( g, b ) );
	float cMax = max( r, max( g, b ) );

	l = ( cMax + cMin ) / 2.0;
	if ( cMax > cMin ) {
		float cDelta = cMax - cMin;
        
        //s = l < .05 ? cDelta / ( cMax + cMin ) : cDelta / ( 2.0 - ( cMax + cMin ) ); Original
		s = l < .0 ? cDelta / ( cMax + cMin ) : cDelta / ( 2.0 - ( cMax + cMin ) );
        
		if ( r == cMax ) {
			h = ( g - b ) / cDelta;
		} else if ( g == cMax ) {
			h = 2.0 + ( b - r ) / cDelta;
		} else {
			h = 4.0 + ( r - g ) / cDelta;
		}

		if ( h < 0.0) {
			h += 6.0;
		}
		h = h / 6.0;
	}
	return vec3( h, s, l );
}

vec3 hsl2rgb( in vec3 c )
{
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );

    return c.z + c.y * (rgb-0.5)*(1.0-abs(2.0*c.z-1.0));
}

// input normalized color
vec3 roundHSL10(in vec3 c ) {
  int newL = int(c.z * 10.f);
  return vec3(c.xy, newL/10.f);
}

// quantize the color
void toonifyColorHSL(inout vec3 c) {
  c.x = round(c.x*10.f) / 10.f;
  c.y = round(c.y*10.f) / 10.f;
  c.z = round(c.z*10.f) / 10.f;
}

// reduce saturation, clamp lightness
void pastelizeHSL(inout vec3 c) {
  if(c.y > 0.4) c.y = 0.4;

  if(c.z > 0.4) c.z = 0.8;
  else c.z = 0.5;
}

void main() {
	vec3 colorHSL;
	if(useTexture) {
	  colorHSL = rgb2hsl(texture( fragmentTextureSampler, UV ).rgb);
	} else {
	  colorHSL = rgb2hsl(vsColor);
	}
    
	colorHSL.z *= max(0.1, intensity);
	//toonifyColorHSL(colorHSL);
	//pastelizeHSL(colorHSL);
	color = vec4(hsl2rgb(colorHSL), 1.0);

	// debugging
	//color = vec4(0,1.f,0,1.f);
	//if(intensity < 0.05) {
	//  color = vec4(0,1.f,0,1.f);
	//}
	//vec3 vn = normalize(varyingNormal);
	//color = vec4( (vn.r + 1) / 2.f, (vn.g + 1) / 2.f, (vn.b+1) / 2.f, 1.f);
}
