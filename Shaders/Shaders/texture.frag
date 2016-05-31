#version 330 core
in vec3 vNormal;
in vec2 TexCoord;
in vec3 vPosition;

out vec4 color;

// Texture samplers
uniform sampler2D wallTexture;


void main()
{

	vec3 frag_Color = texture(wallTexture, TexCoord).rgb;

	color.rgb = frag_Color;

}