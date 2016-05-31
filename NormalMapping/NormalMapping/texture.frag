#version 330 core
in vec3 vNormal;
in vec2 TexCoord;
in vec3 vPosition;

in vec3 NBT_lightDir;
in vec3 NBT_viewDir;

in vec3 vDebug;

out vec4 color;

// Texture samplers
uniform sampler2D wallTexture;
uniform sampler2D wallNormal;

uniform vec3 lightPos;

const float PI  = 3.141592653589793238462643383;


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
//	vec3 N = normalize(vNormal);
//	vec3 L = normalize(vec3(1,-0.5,0.2));
//
//	float NoL = clamp(dot(N,L),0.0,1.0);
//	float diffuse = max(0.0,0.75*NoL + 0.25);
//
//	color = vec4(vec3(diffuse),1.0);

	vec3 N = 2.0 *  texture2D(wallNormal,TexCoord).rgb - 1.0;
	vec3 L = NBT_lightDir;
	vec3 V = NBT_viewDir;
	vec3 H = normalize(L + V);

	float NoV = clamp(dot(N,V),0.0,1.0);
	float NoL = clamp(dot(N,L),0.0,1.0);

	///////////////////////////////////
	// metalness blending
	float uMaterialMetalness = 0.5;
	vec3 uF_0 = vec3(1.0,0.71,0.29); // gold
	float uRoughness = 0.2;
	/////////////////////////////////

	// Point Light - Specular
	vec3 F = F_Schlick(uF_0,L,H);
	
	float alpha_tr = uRoughness*uRoughness;
	float D = D_tr(N,H,alpha_tr);

	float k = (uRoughness + 1.0)*(uRoughness + 1.0)/8.0;
	float G = G_l(N,L,k)*G_l(N,V,k);
	
	vec3 specularBRDF_PtL = D*F*G/(4.0*max(dot(N,L),0.0)*max(dot(N,V),0.0)+0.001);
	
	// Point Light - Diffuse
	vec3 diffuseBRDF_PtL = texture2D(wallTexture,TexCoord).rgb / PI;

	// metalness blending
	vec3 BRDF_PtL = mix(diffuseBRDF_PtL,specularBRDF_PtL, uMaterialMetalness);


	color.rgb = PI*BRDF_PtL*NoL;




//	color.rgb = vDebug;
//color.rgb = vec3(dot(lightPos,vPosition));
}
