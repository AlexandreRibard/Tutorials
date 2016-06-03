#version 400 core

layout (quads, equal_spacing, ccw) in;


void main()
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;


	// patch vertices (control points)
	vec4 p0 = gl_in[0].gl_Position;
	vec4 p1 = gl_in[1].gl_Position;
	vec4 p2 = gl_in[2].gl_Position;
	vec4 p3 = gl_in[3].gl_Position;

	// linerar interpolation
	gl_Position = 
		p0 * (1-u) * (1-v) + 
		p1 * u * (1-v) +
		p3 * v* (1-u) +
		p2 * u * v;
}