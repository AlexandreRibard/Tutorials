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

void main()
{


    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);

	vNormal = normalize(normalMatrix*vec4(normal,0.0f)).rgb;
    TexCoord = texCoord;

	vPosition = normalize(vec3(viewMatrix * modelMatrix * vec4(position, 1.0f)));


}