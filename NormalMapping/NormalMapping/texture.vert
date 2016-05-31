#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangeant;
layout (location = 4) in vec3 bitangeant;

out vec3 vNormal;
out vec2 TexCoord;
out vec3 vPosition;

out vec3 NBT_lightDir;
out vec3 NBT_viewDir;

out vec3 vDebug;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

uniform vec3 lightPos;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
   
	vNormal = normalize(normalMatrix*vec4(normal,1.0f)).rgb;
    TexCoord = texCoord;

	vPosition = vec3(modelMatrix * vec4(position, 1.0f));

	vec3 vTangeant = normalize((normalMatrix*vec4(tangeant,1.0f)).rgb);
	vec3 vBiTangeant = normalize((normalMatrix*vec4(bitangeant,1.0f)).rgb);

	mat3 toFrenetLocalSpace = mat3(
	vTangeant.x, vBiTangeant.x, vNormal.x,
	vTangeant.y, vBiTangeant.y, vNormal.y,
	vTangeant.z, vBiTangeant.z, vNormal.z);


	vDebug = normalize(lightPos - vPosition);

	NBT_lightDir = normalize(toFrenetLocalSpace * (lightPos - vPosition));
	NBT_viewDir = toFrenetLocalSpace * normalize(vec3(0,0,3)-vPosition);

	vDebug = normalize(toFrenetLocalSpace*(lightPos - vPosition));
	vDebug = NBT_lightDir;
}