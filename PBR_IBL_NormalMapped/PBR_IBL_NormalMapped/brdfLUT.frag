#version 330 core

out vec4 color;


in vec2 TexCoords;

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
vec3 importanceSampling_GGX(vec2 Xi, float roughness)
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

	return H;
}



float G_Smith(const in float roughness, const in float NoV, const in float NoL)
{
	float k = (roughness + 1.0)*(roughness + 1.0)/8.0;

	float Gl_l = NoL / (NoL*(1.0-k) + k);
	float Gl_v = NoV / (NoV*(1.0-k) + k);

	return Gl_l * Gl_v;
}

float GSmith(float roughness, float NoV, float NoL)
{
	float alpha_tr = roughness*roughness;
	
	float visV = NoV + sqrt(NoV * (NoV - NoV * alpha_tr) + alpha_tr);
	float visL = NoL + sqrt(NoL * (NoL - NoL * alpha_tr) + alpha_tr);
	
	return 1.0f / (visV * visL);

//	float k = (roughness * roughness) * 0.5;
//
//	float Gl_l = 1.0 / (NoL*(1.0-k) + k);
//	float Gl_v = 1.0 / (NoV*(1.0-k) + k);
//
//	return 0.25 * Gl_l * Gl_v;
}


vec2 integrateBRDF(float roughness, float NoV)
{
	vec3 V = vec3(sqrt(1.0 - NoV*NoV),0.0,NoV);
	
	float A = 0.0;
	float B = 0.0;

	vec2 r = vec2(0.0);

	const uint nSamples = uint(1024);
	for(uint i = uint(0); i < nSamples; i++){

		vec2 Xi = Hammersley2D(i,nSamples);
		vec3 H = importanceSampling_GGX(Xi,roughness);
		vec3 L = 2 * dot(V,H) * H - V;

		float NoL = clamp(L.z,0.0,1.0);
		float NoH = clamp(H.z,0.0,1.0);
		float VoH = clamp(dot(V,H),0.0,1.0);

		if (NoL > 0.0)
		{

			float k = (roughness + 1.0)*(roughness + 1.0)/8.0;
			float Gl_l = NoL / (NoL*(1.0-k) + k);
			float Gl_v = NoV / (NoV*(1.0-k) + k);

			float G = Gl_l*Gl_v;

			float G_Vis = G * VoH / (NoH * NoV);
			float Fc = pow(1.0-VoH,5.0);

			A += (1.0-Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}

	//return  r/float(nSamples);
	return  vec2(A,B)/nSamples;
}




vec2 integrateBRDF_v2(float roughness, float NoV)
{
	const uint sampleNum = uint(1024);

	vec3 V = vec3(sqrt(1.0 - NoV*NoV), 0.0, NoV);


	float scale = 0.0f;
	float bias = 0.0f;


	for (uint i = uint(0); i < sampleNum; ++i)
	{
		vec2 Xi = Hammersley2D(i, sampleNum);
		vec3 H = importanceSampling_GGX(Xi, roughness);

		vec3 L = H * 2.0 * dot(V, H) - V;

		float NoL = clamp(L.z,  0.0, 1.0);
		float NoH = clamp(H.z,  0.0, 1.0);
		float VoH = clamp(dot(V, H), 0.0, 1.0);

		if (NoL > 0.0f)
		{
			float G = GSmith(roughness, NoV, NoL);
			float G_Vis = G * NoL * (4.0f * VoH / NoH);
			float Fc = pow(1.0f - VoH, 5.0f);

			scale += (1.0f - Fc) * G_Vis;
			bias += Fc * G_Vis;
		}
	}
	scale /= sampleNum;
	bias /= sampleNum;

	return vec2(scale, bias);
}


void main()
{ 
	
	vec2 fragRelativeCoord = gl_FragCoord.rg * uInverseResolution;


	float roughness = fragRelativeCoord.x;
	float NoV = fragRelativeCoord.y;
	
	color.rg = integrateBRDF(roughness,NoV);

}