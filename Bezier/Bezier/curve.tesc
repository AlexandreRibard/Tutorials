#version 400 core
layout (vertices = 4) out;

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	gl_TessLevelOuter[0] = float(1);

	gl_TessLevelOuter[1] = float(30);
}