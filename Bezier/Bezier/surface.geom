#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec3 tePosition[];
in vec3 teNormal[];

out vec3 gPosition;
out vec3 gNormal;
noperspective out vec3 gEdgeDistance;



void main() {

	mat3 viewportMatrix = mat3(vec3(1920,0,0),vec3(0,1080,0),vec3(0,0,1));

	// Compute each triangle's position in viewport space
    // transform each vertex to viewport space
	vec3 p0 = vec3(gl_in[0].gl_Position / gl_in[0].gl_Position.w);
	vec3 p1 = vec3(gl_in[1].gl_Position / gl_in[1].gl_Position.w);
	vec3 p2 = vec3(gl_in[2].gl_Position / gl_in[2].gl_Position.w);
	p0 = viewportMatrix*p0;
	p1 = viewportMatrix*p1;
	p2 = viewportMatrix*p2;



	// find triangle haltitude
	// => distance of triangle vertices to opposit edge
	float a = length(p1 - p2);
	float b = length(p2 - p0);
	float c = length(p1 - p0);
	float alpha = acos( (b*b + c*c - a*a)/(2.0*b*c) );
	float beta = acos( (a*a + c*c - b*b)/(2.0*a*c) );

	float ha = abs( c * sin(beta) );
	float hb = abs( c * sin(alpha) );
	float hc = abs( b * sin(alpha) );

	// send out the triangle along with the edge distance
	gPosition = tePosition[0];
	gNormal = teNormal[0];
	gEdgeDistance = vec3(ha,0,0);

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

	gPosition = tePosition[1];
	gNormal = teNormal[1];
	gEdgeDistance = vec3(0,hb,0);

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    
	gPosition = tePosition[2];
	gNormal = teNormal[2];
	gEdgeDistance = vec3(0,0,hc);

	gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}  