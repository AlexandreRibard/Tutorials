#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

in vec3 vNormal[];
in vec3 vPosition[];
in vec2 TexCoord[];

out vec3 gNormal;
out vec3 gPosition;

const float MAGNITUDE = 0.4f;

void generateNormal()
{
	// triangle center of gravity
	vec4 tBarycenter = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position)/3.0;
	// triangle normal by adverging out vertex normal
	vec3 tNormal = (vNormal[0] + vNormal[1] + vNormal[2])/3.0;

	// emmiting normal line strip
	// start: triangle center of gravity
	// end: center of gravity translated in the direction of the triangle's normal
	gNormal = tNormal;
    gl_Position = tBarycenter;
	gPosition = gl_Position.rgb;
    EmitVertex();

	gNormal = tNormal;
    gl_Position = tBarycenter + vec4(tNormal, 0.0f) * MAGNITUDE;
	gPosition = gl_Position.rgb;
    EmitVertex();
    EndPrimitive();
}


void main() {
	
	
	// generate triangle normal as a line strip
	generateNormal(); 

    EndPrimitive();
}  