#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec3 vNormal;
out vec3 vPosition;

out vec2 TexCoord;

out vec4 Lvector;
out vec4 Vvector;
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


/*
// double floating point precision
const vec3 L_0_0 = vec3(1.57598,1.62688,1.53975);
const vec3 L_1_m1 = vec3(-0.00508837,0.101705,0.288045);
const vec3 L_1_0 = vec3(-0.104335,-0.0604074,-0.0183638);
const vec3 L_1_1 = vec3(-0.0599984,-0.0655757,-0.07151);
const vec3 L_2_m2 = vec3(0.238924,0.279482,0.346624);
const vec3 L_2_m1 = vec3(-0.15678,-0.157747,-0.140031);
const vec3 L_2_0 = vec3(-0.188055,-0.127402,-0.0781848);
const vec3 L_2_1 = vec3(-0.0239701,-0.0413113,-0.0427444);
const vec3 L_2_2 = vec3(-0.0647209,-0.0344826,-0.0311779);
*/


// floating point precision
//const vec3 L_0_0 = vec3(1.47738,1.55428,1.45308);
//const vec3 L_1_m1 = vec3(-0.00576314,0.100371,0.291455);
//const vec3 L_1_0 = vec3(-0.104538,-0.0604127,-0.0178978);
//const vec3 L_1_1 = vec3(-0.0603729,-0.0656247,-0.0692839);
//const vec3 L_2_m2 = vec3(0.240448,0.286972,0.353656);
//const vec3 L_2_m1 = vec3(-0.15708,-0.157839,-0.140334);
//const vec3 L_2_0 = vec3(-0.193257,-0.127684,-0.0755121);
//const vec3 L_2_1 = vec3(-0.0240163,-0.0415702,-0.0426194);
//const vec3 L_2_2 = vec3(-0.0597213,-0.0348059,-0.0349329);



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
    vNormal = normal;
	//vNormal = (projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0f)).xyz;


	vNormal = normalize(normalMatrix*vec4(normal,0.0f)).rgb;
    TexCoord = texCoord;

	vPosition = normalize(vec3(viewMatrix * modelMatrix * vec4(position, 1.0f)));

	vec4 eyePos = vec4(0.0,0.0,0.0,1.0);
    Vvector = normalize(eyePos - (viewMatrix * modelMatrix * vec4(position,1.0)));

    Lvector = normalize(vec4(lighDir,1.0) - (viewMatrix * modelMatrix * vec4(position,1.0)));

	
	vec3 worldNormal = normalize( ( vec4( vNormal, 0.0 ) * viewMatrix ).xyz );
        
	vColor = IBL_diffuse(worldNormal);



	/*
	// 3D plane
	// n (a,b,c)
	// A (xA,yA,zA)
	vec3 n = vec3(0,1,0);
	vec3 A = vec3(0,0,0);
	float d = -dot(A,n);


	// M(x,y,z) is in P <=> n.AM = 0
	// plane: ax + by + cz + d

	// dM,P = abs(dot(M,n) + d) / length(n)
	// signed distance: (dot(vPosition,n) + d) / length(n)
	float d_frag_plane = (dot(vPosition,n) + d) / length(n);

	gl_ClipDistance[0] = d_frag_plane;
	*/
}