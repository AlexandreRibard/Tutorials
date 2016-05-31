#version 330 core
layout (location = 0) out vec4 G_PositionDepth;
layout (location = 1) out vec3 G_Normal;
layout (location = 2) out vec3 G_Color;

in vec3 vPosition;
in vec2 TexCoords;
in vec3 vNormal;

const float NEAR = 1.0f; // projection matrix's near plane
const float FAR = 100.0f; // projection matrix's far plane
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));	
}

void main()
{    
    // Store the fragment position vector in the first gbuffer texture
    G_PositionDepth.rgb = vPosition;
	G_PositionDepth.w = LinearizeDepth(gl_FragCoord.z); 

    // Also store the per-fragment normals into the gbuffer
    G_Normal = normalize(vNormal);
    // And the per-fragment color
	G_Color = vec3(TexCoords,0.0);
    
}