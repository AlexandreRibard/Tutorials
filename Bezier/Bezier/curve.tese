#version 400 core

layout (isolines) in;

void main()
{
	// tessellation u coordinate
	float u = gl_TessCoord.x;

	// patch vertices (control points)
	vec3 p0 = gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_in[1].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz;
	vec3 p3 = gl_in[3].gl_Position.xyz;

	float u2 = u * u;

	float w = 1.0 - u;
	float w2 = w * w;

	float b0 = w * w2;
	float b1 = 3.0 * w2 * u;
	float b2 = 3.0 * w * u2;
	float b3 = u * u2;

	vec3 p = p0*b0 + p1*b1 + p2*b2 + p3*b3;

	gl_Position = vec4(p,1.0);

}