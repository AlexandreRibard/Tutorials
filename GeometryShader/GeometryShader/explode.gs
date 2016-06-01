#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 vNormal[];
in vec3 vPosition[];
in vec2 TexCoord[];

out vec3 gNormal;
out vec3 gPosition;
out vec2 gTexCoord;

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 0.1f;
    vec3 direction = normal * magnitude; 
    return position + vec4(direction, 0.0f);
} 

vec3 computeTriangleNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}  
vec3 adverageNormal()
{
	return (vNormal[0] + vNormal[1] + vNormal[2])/3.0;
}

void main() {
	vec3 tNormal = computeTriangleNormal();
	tNormal = adverageNormal();
    
	gNormal = vNormal[0];
	gPosition = vPosition[0];
	gTexCoord = TexCoord[0];

    gl_Position = explode(gl_in[0].gl_Position, tNormal);
    EmitVertex();

	gNormal = vNormal[1];
	gPosition = vPosition[1];
	gTexCoord = TexCoord[1];

    gl_Position = explode(gl_in[1].gl_Position,tNormal);
    EmitVertex();
    
	gNormal = vNormal[2];
	gPosition = vPosition[2];
	gTexCoord = TexCoord[2];
	
	gl_Position = explode(gl_in[2].gl_Position,tNormal);
    EmitVertex();

    EndPrimitive();
}  