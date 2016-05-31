#version 430 core

uniform sampler2D wallNormal;
uniform samplerCube skybox;
uniform sampler2D IntegrateBRDF;
uniform sampler2D IBLequirectangularEnvMap;
uniform vec3 lightPos;


uniform vec3 uF_0;
uniform float uRoughness;
uniform float uMaterialMetalness;

in mat4 vViewMatrix;
in mat3 inv_vToFrenetLocalSpace;

in vec3 vNormal;
in vec3 vPosition;

in vec3 NBT_lightDir;
in vec3 NBT_viewDir;

in vec2 TexCoord;

in vec4 Lvector;
in vec4 Vvector;
in vec3 vColor;



out vec4 fColor;

const float PI  = 3.141592653589793238462643383;
const float PI2 = 6.283185307179586476925286767;
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


vec4 RadialLookup(sampler2D radialTex, vec3 viewDirection, float mipLevel)
{
	 // normally:
	 // x = r sin(phi) cos(theta)
	 // y = r sin(theta) sin(phi)
	 // z = r cos(theta)
	 // => but with opengl mapping,
	 // x = cos(theta) * cos(phi);
	 // y = sin(phi);
	 // z = sin(theta) * cos(phi);
	 float phi = atan(viewDirection.z, viewDirection.x);
	 float theta = acos(viewDirection.y);
	 vec2 sphereCoords = vec2(phi/PI2 + 0.5, theta/PI);

	 return textureLod(radialTex, sphereCoords, mipLevel);
}



void main()
{

	
	vec3 N = 2.0 *  texture2D(wallNormal,TexCoord).rgb - 1.0;
	N = normalize(N);
	N = normalize(inv_vToFrenetLocalSpace * N);

	vec3 worldNormal = normalize((vec4(N,0.0f) * vViewMatrix).rgb);


	//vec3 L = normalize(NBT_lightDir);
	//vec3 V = normalize(NBT_viewDir);
	vec3 V = normalize(vec3(0,0,3)-vPosition);
	
	vec3 I = normalize(-V);
	vec3 R = reflect(I, normalize(N));


	float NoV = clamp(dot(N,V),0.0,1.0);



	vec3 uDiffuseAlbedo = vec3(1.0);



	// IBL - Diffuse
	vec3 indirectDiffuse = IBL_diffuse(worldNormal) * uDiffuseAlbedo / PI;
	

	// IBL - Specular

	vec3 EnvBRDF = texture2D( IntegrateBRDF, vec2(uRoughness, NoV) ).rgb;


	int nMips =  textureQueryLevels(skybox);
	int lodLevel = int(round(uRoughness * float(nMips)));
	vec3 IBLEnvMapColor = RadialLookup(IBLequirectangularEnvMap,R,lodLevel).rgb;

	vec3 indirectSpecular = IBLEnvMapColor * (uF_0 * EnvBRDF.x + EnvBRDF.y);


	fColor.rgb = indirectSpecular;

}