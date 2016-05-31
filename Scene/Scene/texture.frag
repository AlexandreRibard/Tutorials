#version 330 core
in vec3 vNormal;
in vec2 TexCoord;
in vec3 vPosition;

out vec4 color;




void main()
{
	vec3 N = normalize(vNormal);
	vec3 L = normalize(vec3(1,-0.5,0.2));

	float NoL = clamp(dot(N,L),0.0,1.0);
	float diffuse = max(0.0,0.75*NoL + 0.25);

	color = vec4(vec3(diffuse),1.0);
}