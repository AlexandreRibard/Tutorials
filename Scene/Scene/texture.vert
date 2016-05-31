#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 vNormal;
out vec2 TexCoord;
out vec3 vPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;


void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
   
	vNormal = (normalMatrix*vec4(normal,1.0f)).rgb;
    TexCoord = texCoord;

	vPosition = vec3(modelMatrix * vec4(position, 1.0f));

}