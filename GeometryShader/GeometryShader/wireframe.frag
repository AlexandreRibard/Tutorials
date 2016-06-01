#version 430 core


uniform vec3 lightPos;



in vec3 gNormal;
in vec3 gPosition;
in vec2 gTexCoord;
in vec3 gEdgeDistance;



out vec4 fColor;



void main()
{

	// Wireframe using geometry shader
	
	fColor.rgb = gEdgeDistance;
	// find the smallest distance
	float d = min(gEdgeDistance.x,min(gEdgeDistance.y,gEdgeDistance.z));
	fColor.rgb = vec3(d);

	// determin mixing factor
	float lwidth = 1.2;
	float mixVal = smoothstep(lwidth - 1.0, lwidth + 1.0, d);
	fColor.rgb = vec3(mixVal);

	vec3 lColor = vec3(0.0,0.0,0.0);
	vec3 mColor = vec3(1.0,1.0,1.0);
	fColor.rgb = mix(lColor,mColor,mixVal);
	
}