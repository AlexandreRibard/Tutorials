#version 400 core
out vec4 fColor;

in vec3 gPosition;
noperspective in vec3 gEdgeDistance;
in vec3 gNormal;

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
	mColor = gNormal;
	fColor.rgb = mix(lColor,mColor,mixVal);
	
}