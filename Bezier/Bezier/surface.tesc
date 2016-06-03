#version 400 core
layout (vertices = 24) out;

const int MAX_TESSLEVEL = 128;
const int MIN_TESSLEVEL = 2;
const float maxDepth = 100.0;
const float minDepth = 1.0;


uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

float computeDepth(float d)
{
	float z = abs(d);
	float near = minDepth;
	float far = maxDepth;
	return  clamp( (z - near)/(far - near), 0.0, 1.0);
}

void main()
{
	// position in camera space
	vec4 p = viewMatrix * modelMatrix * gl_in[gl_InvocationID].gl_Position;

	float dist = length(vec3(0,0,3) - p.xyz);
	float z = abs(p.z);
	z = dist;
	float near = minDepth;
	float far = maxDepth;
	float depth = clamp( (1/z - 1/near)/(1/far - 1/near), 0.0, 1.0);


	float tessLevel = mix(MAX_TESSLEVEL, MIN_TESSLEVEL, depth);

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	gl_TessLevelOuter[0] = float(tessLevel);
	gl_TessLevelOuter[1] = float(tessLevel);
	gl_TessLevelOuter[2] = float(tessLevel);
	gl_TessLevelOuter[3] = float(tessLevel);


	gl_TessLevelInner[0] = float(tessLevel);
	gl_TessLevelInner[1] = float(tessLevel);

}