#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 vNormal;
out vec3 vPosition;

out vec2 TexCoord;



uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

uniform vec3 lighDir;


void main()
{


    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);

	vNormal = normalize(normalMatrix*vec4(normal,0.0f)).rgb;
    TexCoord = texCoord;

	vPosition = normalize(vec3(viewMatrix * modelMatrix * vec4(position, 1.0f)));



	/*
	// 3D plane
	// n (a,b,c)
	// A (xA,yA,zA)
	vec3 n = vec3(0,1,0);
	vec3 A = vec3(0,0,0);
	float d = -dot(A,n);


	// M(x,y,z) is in P <=> n.AM = 0
	// plane: ax + by + cz + d

	// dM,P = abs(dot(M,n) + d) / length(n)
	// signed distance: (dot(vPosition,n) + d) / length(n)
	float d_frag_plane = (dot(vPosition,n) + d) / length(n);

	gl_ClipDistance[0] = d_frag_plane;
	*/
}