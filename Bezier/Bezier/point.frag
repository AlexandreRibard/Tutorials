#version 330 core
out vec4 fColor;

float aastep(float threshold, float value) {
  float afwidth = 0.7 * length(vec2(dFdx(value), dFdy(value)));

  return smoothstep(threshold-afwidth, threshold+afwidth, value);
}

void main()
{
	
	// calculate uv coordinate from texture coordinates
    vec2 UV  = 2.0*gl_PointCoord- vec2(1.0);    
    float mag = length(UV);
	float radius = 1.0;
    if (aastep(radius, mag) > 0.5) discard;   // discard pixels outside unit circle
   

    fColor = vec4(1.0f, 0.0f, 0.0f, 1.0f); 

}  