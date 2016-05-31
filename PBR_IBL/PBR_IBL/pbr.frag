#version 430 core

uniform samplerCube skybox;
uniform sampler2D IntegrateBRDF;
uniform sampler2D IBLequirectangularEnvMap;
uniform vec3 lightPos;

uniform int importanceSampling;

uniform vec3 uF_0;
uniform float uRoughness;
uniform float uMaterialMetalness;


in vec3 vNormal;
in vec3 vPosition;

in vec2 TexCoord;

in vec4 Lvector;
in vec4 Vvector;
in vec3 vColor;



out vec4 fColor;

const float PI  = 3.141592653589793238462643383;
const float PI2 = 6.283185307179586476925286767;
const int SamplesCount = 1;

float rand(vec2 co){
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float F_Schlick(float F_0, vec3 l, vec3 h) {
	float F = (1.0-F_0);
         
	float l_dot_h = max(dot(l,h),0.0);
	F *= pow(2.0,(-5.55473*l_dot_h-6.98316)*l_dot_h);

	F += F_0;

	return F;
}

vec3 F_Schlick(vec3 F_0, vec3 l, vec3 h) {
	vec3 F = (1.0-F_0);
         
	float l_dot_h = max(dot(l,h),0.0);
	F *= pow(2.0,(-5.55473*l_dot_h-6.98316)*l_dot_h);

	F += F_0;

//	F = F_0 + (1.0-F_0) * vec3( 1.0 - pow(max(0.0,dot(l,h)),5.0) );
//	F = F_0 + (1.0-F_0) * vec3( pow(1.0-max(0.0,dot(l,h)),5.0) );


	return F;
}

float D_tr(vec3 n, vec3 v, float alpha_tr) {
	float D = 0.0;


	float alpha_tr2 = alpha_tr*alpha_tr;
	float n_dot_v = max(dot(n,v),0.0);
	float var = (n_dot_v*n_dot_v)*(alpha_tr2-1.0) + 1.0;
	D = alpha_tr2/(PI*var*var);

	return D;
}

// Smith mixed with Schlick-Beckmann
float G_l(vec3 n, vec3 v, float k) {
    float G = 0.0;

    float n_dot_v = max(dot(n,v),0.0);

    G = n_dot_v / (n_dot_v*(1.0-k) + k);

    return G;
}

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

vec3 specularIBL(vec3 specularColor, float roughness, vec3 N, vec3 V)
{
	vec3 specularLighting;

	const uint nSamples = uint(1024);

	for (uint i = uint(0); i < nSamples; i++)
	{
		vec2 Xi = Hammersley2D(i,nSamples);
		vec3 H = importanceSampling_GGX(Xi,roughness,N);

		vec3 L = 2.0 * dot(V,H) * H - V;

		float NoV = clamp(dot(N,V),0.0,1.0);
		float NoL = clamp(dot(N,L),0.0,1.0);
		float NoH = clamp(dot(N,H),0.0,1.0);
		float VoH = clamp(dot(V,H),0.0,1.0);

		if (NoL > 0)
		{
			vec3 sampleColor = textureLod(skybox,L,0).rgb;

			float k = (roughness + 1.0)*(roughness + 1.0)/8.0;

			float Gl_l = NoL / (NoL*(1.0-k) + k);
			float Gl_v = NoV / (NoV*(1.0-k) + k);

			float G = Gl_l*Gl_v;
			float Fc = pow(1.0 - VoH, 5.0);
			vec3 F = (1.0 - Fc) * specularColor + Fc;

			// incident light = sampleColor * NoL;
			// microfacet specular = D*G*F / (4.0*NoL*NoV)
			// pdf = D*NoH / (4*VoH)

			specularLighting += sampleColor * F * G * VoH / (NoH * NoV);
		}


	}
		
	return specularLighting/nSamples;
}


vec3 splitSum_1(float roughness, vec3 R)
{
	vec3 N = R;
	vec3 V = R;

	vec3 prefilteredColor = vec3(0.0);
	float totalWeight = 0.0;

	
	const uint nSamples = uint(1024);

	for (uint i = uint(0); i < nSamples; i++)
	{
		vec2 Xi = Hammersley2D(i,nSamples);
		vec3 H = importanceSampling_GGX(Xi,roughness,N);

		vec3 L = 2.0 * dot(V,H) * H - V;

		float NoL = clamp(dot(N,L),0.0,1.0);
		if (NoL > 0.0){
			prefilteredColor += textureLod(skybox,L,0).rgb * NoL;
			totalWeight += NoL;
		}
	}
	
	return prefilteredColor / totalWeight;
}

vec2 splitSum_2(float roughness, float NoV)
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
	
	return  vec2(A,B)/nSamples;
}


vec3 varianceIBL(float roughness, vec3 R)
{
	vec3 N = R;
	vec3 V = R;

	vec3 expectedValue = vec3(0.0);
	vec3 sigma2 = vec3(0.0);
	float totalWeight = 0.0;

	
	const uint nSamples = uint(1024);

	for (uint i = uint(0); i < nSamples; i++)
	{
		vec2 Xi = Hammersley2D(i,nSamples);
		vec3 H = importanceSampling_GGX(Xi,roughness,N);

		vec3 L = 2.0 * dot(V,H) * H - V;

		float NoL = clamp(dot(N,L),0.0,1.0);
		if (NoL > 0.0){
			expectedValue += textureLod(skybox,L,0).rgb * NoL;
			totalWeight += NoL;
		}
	}
	expectedValue /= totalWeight;
	
	totalWeight = 0.0;
	for (uint i = uint(0); i < nSamples; i++)
	{
		vec2 Xi = Hammersley2D(i,nSamples);
		vec3 H = importanceSampling_GGX(Xi,roughness,N);

		vec3 L = 2.0 * dot(V,H) * H - V;

		float NoL = clamp(dot(N,L),0.0,1.0);
		if (NoL > 0.0){
			sigma2 += pow(textureLod(skybox,L,0).rgb * NoL - expectedValue,vec3(2.0));
			totalWeight += NoL;
		}
	}

	return sigma2 / totalWeight;
}

vec3 varianceBRDF(float roughness, float NoV, vec3 F_0)
{

		vec3 V = vec3(sqrt(1.0 - NoV*NoV),0.0,NoV);


		vec3 expectedValue = vec3(0.0);
		vec3 sigma2 = vec3(0.0);


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
				float Fc = pow(1.0 - VoH, 5.0);
				vec3 F = (1.0 - Fc) * F_0 + Fc;


				expectedValue += F * G * VoH / (NoH * NoV);

			}
		
		}
	
		expectedValue /= nSamples;

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
				float Fc = pow(1.0 - VoH, 5.0);
				vec3 F = (1.0 - Fc) * F_0 + Fc;


				sigma2 += pow(F * G * VoH / (NoH * NoV) - expectedValue,vec3(2.0));
			}
		
		}


		return sigma2/nSamples;

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


float interpolate( float val, float y0, float x0, float y1, float x1 ) {
    return (val-x0)*(y1-y0)/(x1-x0) + y0;
}

float base( float val ) {
    if ( val <= -0.75 ) return 0;
    else if ( val <= -0.25 ) return interpolate( val, 0.0, -0.75, 1.0, -0.25 );
    else if ( val <= 0.25 ) return 1.0;
    else if ( val <= 0.75 ) return interpolate( val, 1.0, 0.25, 0.0, 0.75 );
    else return 0.0;
}

float red( float gray ) {
    return base( gray - 0.5 );
}
float green( float gray ) {
    return base( gray );
}
float blue( float gray ) {
    return base( gray + 0.5 );
}

vec3 colorGrading(float t)
{
	float a = pow(t,1.0/2.2);
	a = t;
	float gray = 2*a-1; // gray in -1,1
	return vec3(red(gray),green(gray),blue(gray));
}



void main()
{



	vec3 cameraPos = vec3(0.0,0.0,3.0);
	vec3 I = normalize(vPosition - cameraPos);
	vec3 R = reflect(I, normalize(vNormal));

    vec3 envColor = texture(skybox, R).rgb;


	vec3 N = normalize(vNormal);
	vec3 L = normalize(lightPos - vPosition);
	vec3 V = normalize(vec3(0,0,3)-vPosition);
	vec3 H = normalize(L + V);

	float NoV = clamp(dot(N,V),0.0,1.0);

	// metalness blending
//	float uMaterialMetalness = 0.9;

	// Fresnel
//	vec3 uF_0 = vec3(1.0,0.71,0.29); // gold
//	uF_0 = vec3(0.95,0.64,0.54); // copper
//	uF_0 = vec3(250.0/255.0,230.0/255.0,150.0/255.0); // brass
//	uF_0 = vec3(0.95,0.93,0.88); // silver
	
//	uF_0 = vec3(0.1,0.1,0.1); // plastic (high)
//	uF_0 = vec3(70.0/255.0,130.0/255.0,180.0/255.0); // steel blue
//	uF_0 = vec3(189.0/255.0,32.0/255.0,49.0/255.0); // cardinale red

	//uF_0 = vec3(0.5); // copper


	
//	float uRoughness = 0.2;

	vec3 uDiffuseAlbedo = uF_0;
	//uDiffuseAlbedo = vec3(121.0/255.0,76.0/255.0,19.0/255.0);
	//uDiffuseAlbedo = vec3(0.95,0.93,0.88);
//	uDiffuseAlbedo = vec3(224.0/255.0,17.0/255.0,95.0/255.0);

	uDiffuseAlbedo = vec3(1.0); // copper





	// Point Light - Specular
	vec3 F = F_Schlick(uF_0,L,H);
	
	float alpha_tr = uRoughness*uRoughness;
	float D = D_tr(N,H,alpha_tr);

	float k = (uRoughness + 1.0)*(uRoughness + 1.0)/8.0;
	float G = G_l(N,L,k)*G_l(N,V,k);
	
	vec3 specularBRDF_PtL = D*F*G/(4.0*max(dot(N,L),0.0)*max(dot(N,V),0.0)+0.001);
	
	// Point Light - Diffuse
	vec3 diffuseBRDF_PtL = uDiffuseAlbedo / PI;

	// metalness blending
	vec3 BRDF_PtL = mix(diffuseBRDF_PtL,specularBRDF_PtL, uMaterialMetalness);

	// => Specular Point Light
	fColor.rgb = PI*specularBRDF_PtL*max(dot(N,L),0.0);
	// => Diffuse Point Light
	fColor.rgb = PI*diffuseBRDF_PtL*max(dot(N,L),0.0);

	// => Diffuse + Specular Point Light
	fColor.rgb = PI*BRDF_PtL*max(dot(N,L),0.0);


	
	// IBL - Diffuse
	vec3 indirectDiffuse = vColor * uDiffuseAlbedo / PI;



	// IBL - Specular
	vec3 indirectSpecular;

	vec3 EnvBRDF, IBLEnvMapColor;


	if (importanceSampling == 0) {
		EnvBRDF = texture2D( IntegrateBRDF, vec2(uRoughness, NoV) ).rgb;


		int nMips =  textureQueryLevels(skybox);
		int lodLevel = int(round(uRoughness * float(nMips)));
		IBLEnvMapColor = RadialLookup(IBLequirectangularEnvMap,R,lodLevel).rgb;

		indirectSpecular = IBLEnvMapColor * (uF_0 * EnvBRDF.x + EnvBRDF.y);
	} else if (importanceSampling == 1) {
		indirectSpecular = specularIBL(uF_0,uRoughness,N,V);
	} else {
		IBLEnvMapColor = splitSum_1(uRoughness, R);
		EnvBRDF.rg = splitSum_2(uRoughness, NoV);
		indirectSpecular = IBLEnvMapColor * (uF_0 * EnvBRDF.x + EnvBRDF.y);
	}
	fColor.rgb = indirectSpecular;


//	vec3 groundTruth = specularIBL(uF_0,uRoughness,N,V);
//	vec3 error = abs(indirectSpecular - groundTruth);
//	error = colorGrading(error.r);
//	fColor.rgb = vec3(error);

//	vec3 varIBL = varianceIBL(uRoughness, R);
//	vec3 varBRDF = varianceBRDF(uRoughness, NoV, uF_0);
//	vec3 supremum = sqrt(varIBL * varBRDF);
//	supremum = colorGrading(supremum.r);
//	fColor.rgb = vec3(supremum);



	// metalness blending
	fColor.rgb = mix(indirectDiffuse,indirectSpecular, uMaterialMetalness);


	// adding sepcular point light
	vec3 uLightColor = vec3(253.0/255.0,184.0/255.0,19.0/255.0);
	//uLightColor = vec3(176.0/255.0,23.0/255.0,31.0/255.0);
	//uLightColor = vec3(70.0/255.0,130.0/255.0,180.0/255.0);


	float dist_LV = length(L);
	float uLightRadius = 50.0;
	float falloff = clamp(1.0 - pow(dist_LV/uLightRadius,4.0),0.0,1.0);
	falloff *= falloff;
	falloff /= (dist_LV*dist_LV + 1.0);

	float NoL = clamp(dot(L,N),0.0,1.0);
	fColor.rgb += falloff * PI * specularBRDF_PtL * NoL * uLightColor;

}