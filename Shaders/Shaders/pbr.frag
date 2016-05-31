#version 430 core


uniform vec3 lightPos;



in vec3 vNormal;
in vec3 vPosition;

in vec2 TexCoord;



out vec4 fColor;

const float PI  = 3.141592653589793238462643383;
const float PI2 = 6.283185307179586476925286767;


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



void main()
{



	vec3 cameraPos = vec3(0.0,0.0,3.0);
	vec3 I = normalize(vPosition - cameraPos);
	vec3 R = reflect(I, normalize(vNormal));



	vec3 N = normalize(vNormal);
	vec3 L = normalize(lightPos - vPosition);
	vec3 V = normalize(vec3(0,0,3)-vPosition);
	vec3 H = normalize(L + V);

	float NoV = clamp(dot(N,V),0.0,1.0);
	float NoL = clamp(dot(N,L),0.0,1.0);

	// metalness blending
	float uMaterialMetalness = 0.5;

	// Fresnel
	vec3 uF_0 = vec3(1.0,0.71,0.29); // gold
//	uF_0 = vec3(0.95,0.64,0.54); // copper
//	uF_0 = vec3(250.0/255.0,230.0/255.0,150.0/255.0); // brass
//	uF_0 = vec3(0.95,0.93,0.88); // silver
	
//	uF_0 = vec3(0.1,0.1,0.1); // plastic (high)
//	uF_0 = vec3(70.0/255.0,130.0/255.0,180.0/255.0); // steel blue
//	uF_0 = vec3(189.0/255.0,32.0/255.0,49.0/255.0); // cardinale red

	
	float uRoughness = 0.5;

	vec3 uDiffuseAlbedo = uF_0;
	//uDiffuseAlbedo = vec3(121.0/255.0,76.0/255.0,19.0/255.0);
	//uDiffuseAlbedo = vec3(0.95,0.93,0.88);
//	uDiffuseAlbedo = vec3(224.0/255.0,17.0/255.0,95.0/255.0);
	uDiffuseAlbedo = vec3(0.87,0.22,0.22);



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
	fColor.rgb = PI*specularBRDF_PtL*NoL;
	// => Diffuse Point Light
	fColor.rgb = PI*diffuseBRDF_PtL*NoL;

	// => Diffuse + Specular Point Light
	fColor.rgb = PI*BRDF_PtL*NoL;



	// light fallof
	// adding sepcular point light
	vec3 uLightColor = vec3(253.0/255.0,184.0/255.0,19.0/255.0);
	//uLightColor = vec3(176.0/255.0,23.0/255.0,31.0/255.0);
	//uLightColor = vec3(70.0/255.0,130.0/255.0,180.0/255.0);

	float dist_LV = length(L);
	float uLightRadius = 50.0;
	float falloff = clamp(1.0 - pow(dist_LV/uLightRadius,4.0),0.0,1.0);
	falloff *= falloff;
	falloff /= (dist_LV*dist_LV + 1.0);

	fColor.rgb += falloff * PI * BRDF_PtL * NoL * uLightColor;
}