#version 330 core

out vec4 color;


in vec2 TexCoords;


uniform samplerCube skybox;
uniform float uRoughness;
uniform vec2 uInverseResolution;

const float PI = 3.141592653589793238462643383;




// phi = a0/2 + a1/2**2 + ... + ar/2**r+1
// where i = a0 + a1*2 + a2*2**2 +...+ ar*2**r
float radicalInverse_VdC(uint i)
{
	i = (i << 16u) | (i >> 16u);
    i = ((i & 0x55555555u) << 1u) | ((i & 0xAAAAAAAAu) >> 1u);
    i = ((i & 0x33333333u) << 2u) | ((i & 0xCCCCCCCCu) >> 2u);
    i = ((i & 0x0F0F0F0Fu) << 4u) | ((i & 0xF0F0F0F0u) >> 4u);
    i = ((i & 0x00FF00FFu) << 8u) | ((i & 0xFF00FF00u) >> 8u);
    return float(i) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley2D(uint i, uint nSamples)
{
	return vec2(float(i)/float(nSamples), radicalInverse_VdC(i));
}

// Xi is a point on the Hammarsley point set
// Xi = (u,v)
// we then map Xi to the hemmisphere
// unifom mapping: theta = cos-1(1-u)
//				   phi = 2*pi*v
// cossinus mapping: theta = cos-1(sqrt(1-u))
//				     phi = 2*pi*v
vec3 importanceSampling_GGX(vec2 Xi, float roughness, vec3 N)
{
	float alpha_tr = roughness*roughness;

	float u = Xi.x;
	float v = Xi.y;

	float phi = 2 * PI * u;
	float cosTheta = sqrt( (1.0-v)/(1.0+(alpha_tr*alpha_tr -1.0)*v) );
	float sinTheta = sqrt( 1.0 - cosTheta*cosTheta);

	vec3 H;
	H.x = sinTheta * cos(phi);
	H.y = sinTheta * sin(phi);
	H.z = cosTheta;

	// Gramm-Schmitt
	vec3 upVector = abs(N.z) < 0.9999 ? vec3(0.0,0.0,1.0) : vec3(1.0,0.0,0.0);
	vec3 tangentX = normalize(cross(upVector,N));
	vec3 tangentY = cross(N,tangentX);

	// tangent to wrold space
	return tangentX * H.x + tangentY * H.y + N * H.z;
}



void main()
{ 

	//float theta = PI * (2.0*gl_FragCoord.x * uInverseResolution.x - 1.0);
	//float phi = -PI/2.0 * (2.0*gl_FragCoord.y * uInverseResolution.y - 1.0);

	//float rx = cos(theta) * cos(phi);
	//float ry = sin(phi);
	//float rz = sin(theta) * cos(phi);

	//vec3 R = normalize(vec3(rx,ry,rz));

	vec2 thetaphi = (((vec2(gl_FragCoord.xy) * uInverseResolution) * 2.0) - vec2(1.0)) * vec2(3.1415926535897932384626433832795,-1.5707963267948966192313216916398);
	float theta = thetaphi.x;
	float phi = thetaphi.y;
	vec3 R = normalize(vec3(cos(theta) * cos(phi), sin(phi), sin(theta) * cos(phi)));



	vec3 N = R;
	vec3 V = R;

	vec3 prefilteredColor = vec3(0.0);
	float totalWeight = 0.0;


	const uint nSamples = uint(1024);

	for (uint i = uint(0); i < nSamples; i++)
	{
		vec2 Xi = Hammersley2D(i,nSamples);
		vec3 H = importanceSampling_GGX(Xi,uRoughness,N);

		vec3 L = 2.0 * dot(V,H) * H - V;

		float NoL = clamp(dot(N,L),0.0,1.0);
		if (NoL > 0.0){
			prefilteredColor += textureLod(skybox,L,0).rgb * NoL;
			totalWeight += NoL;
		}
	}

	color.rgb = prefilteredColor / totalWeight;
}