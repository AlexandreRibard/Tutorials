#version 330 core

out vec3 frag_depth;

float LinearizeDepth(float depth) 
{
	float near = 1.0; 
	float far  = 100.0; 

    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (z * (far - near) - (far + near));	
}


void main()
{             
	float depth = (gl_FragCoord.z);

	float dx = dFdx(depth);
	float dy = dFdy(depth);

	frag_depth = vec3(depth,pow(depth, 2.0) + 0.25*(dx*dx + dy*dy),0.0);

	//frag_depth = vec3(depth,pow(depth, 2.0),0.0);
} 