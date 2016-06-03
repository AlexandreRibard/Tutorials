#version 400 core
layout (location = 0) in vec2 position;

out vec3 vPosition;


void main()
{
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f); 

	vPosition = normalize( vec3(position.x, position.y, 0.0f) );

}