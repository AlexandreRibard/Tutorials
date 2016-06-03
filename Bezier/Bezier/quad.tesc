#version 400 core
layout (vertices = 4) out;

const int OUTER = 1;
const int INNER = 8;

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	gl_TessLevelOuter[0] = float(OUTER);
	gl_TessLevelOuter[1] = float(OUTER);
	gl_TessLevelOuter[2] = float(OUTER);
	gl_TessLevelOuter[3] = float(OUTER);


	gl_TessLevelInner[0] = float(INNER);
	gl_TessLevelInner[1] = float(INNER);

}