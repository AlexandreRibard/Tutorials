#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangeant;
layout (location = 4) in vec3 bitangeant;

out vec3 vNormal;
out vec3 vPosition;

out vec2 TexCoord;

out vec4 Lvector;
out vec4 Vvector;


out vec3 NBT_lightDir;
out vec3 NBT_viewDir;

out mat4 vViewMatrix;
out mat3 inv_vToFrenetLocalSpace;
out vec3 vColor;


uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

uniform vec3 lighDir;




const int N_SH_COEFFS = 9*3;
uniform vec3 sphericalHarmonics_Coeff[N_SH_COEFFS];

const float c1 = 0.429043 ;
const float c2 = 0.511664 ;
const float c3 = 0.743125 ;
const float c4 = 0.886227 ;
const float c5 = 0.247708 ;


vec3 IBL_diffuse(vec3 v){
    float x = v.x;
    float y = v.y;
    float z = v.z;
	
	vec3 L_0_0 = sphericalHarmonics_Coeff[0];
	vec3 L_1_m1 = sphericalHarmonics_Coeff[1];
	vec3 L_1_0 = sphericalHarmonics_Coeff[2];
	vec3 L_1_1 = sphericalHarmonics_Coeff[3];
	vec3 L_2_m2 = sphericalHarmonics_Coeff[4];
	vec3 L_2_m1 = sphericalHarmonics_Coeff[5];
	vec3 L_2_0 = sphericalHarmonics_Coeff[6];
	vec3 L_2_1 = sphericalHarmonics_Coeff[7];
	vec3 L_2_2 = sphericalHarmonics_Coeff[8];

    vec3 E = c1*L_2_2*(x*x-y*y) + c3*L_2_0*z*z + c4*L_0_0 - c5*L_2_0
            + 2.0*c1*(L_2_m2*x*y + L_2_1*x*z + L_2_m1*y*z)
            + 2.0*c2*(L_1_1*x + L_1_m1*y + L_1_0*z);

    return E;
}


void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f);
	
	
	vNormal = normalize(normalMatrix*vec4(normal,0.0f)).rgb;

    TexCoord = texCoord;

	vec4 eyePos = vec4(0.0,0.0,0.0,1.0);
    Vvector = normalize(eyePos - (viewMatrix * modelMatrix * vec4(position,1.0)));

    Lvector = normalize(vec4(lighDir,1.0) - (viewMatrix * modelMatrix * vec4(position,1.0)));

	
    vec3 worldNormal = normalize( ( vec4( vNormal, 0.0 ) * viewMatrix ).xyz );
	vColor = IBL_diffuse(worldNormal);  
	
	vPosition = vec3(modelMatrix * vec4(position, 1.0f));


	vec3 vTangeant = normalize((normalMatrix*vec4(tangeant,1.0f)).rgb);
	vec3 vBiTangeant = normalize((normalMatrix*vec4(bitangeant,1.0f)).rgb);

	mat3 toFrenetLocalSpace = mat3(
	vTangeant.x, vBiTangeant.x, vNormal.x,
	vTangeant.y, vBiTangeant.y, vNormal.y,
	vTangeant.z, vBiTangeant.z, vNormal.z);

	NBT_lightDir = normalize(toFrenetLocalSpace * (lighDir - vPosition));
	NBT_viewDir = toFrenetLocalSpace * normalize(vec3(0,0,3)-vPosition);

	vViewMatrix = viewMatrix;
	inv_vToFrenetLocalSpace = inverse(toFrenetLocalSpace);

	vPosition = normalize(vec3(viewMatrix * modelMatrix * vec4(position, 1.0f)));
}