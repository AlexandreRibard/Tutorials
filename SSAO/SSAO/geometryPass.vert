#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 vPosition;
out vec2 TexCoords;
out vec3 vNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);

	vNormal = (normalMatrix*vec4(normal,1.0f)).rgb;
    TexCoords = texCoords;
	vPosition = vec3(viewMatrix * modelMatrix * vec4(position, 1.0f));
}