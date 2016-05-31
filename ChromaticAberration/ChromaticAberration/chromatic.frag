#version 330 core
in vec3 vNormal;
in vec2 TexCoord;
in vec3 vPosition;


out vec4 color;

// Texture samplers
uniform samplerCube skybox;

float F_Schlick(float F_0, vec3 l, vec3 h) {
	float F = (1.0-F_0);
         
	float l_dot_h = max(dot(l,h),0.0);
	F *= pow(2.0,(-5.55473*l_dot_h-6.98316)*l_dot_h);

	F += F_0;

	return F;
}

void main()
{

	
    vec3 N = normalize(vNormal);
    vec3 V = normalize(vec3(0,0,3)-vPosition);	
	vec3 I = -V;



	// reflection
	vec3 reflectionDir = reflect(I, N);

	// refraction
	//float eta = 1.0/1.52;
	//vec3 refractionDir = refract(I, N, eta);

	vec4 reflectionColor = texture(skybox,reflectionDir);


	// Chromatic aberration
	// Cauchy's law:
	// n(lamda) = B + C/lamda²
	//	---
	// crown glass B = 1.5046
	//             C = 0.00420 mum²
	// n_crownglass = 1.52
	//	---
	// lamda_blue = 475 nm
	// lamda_green = 510 nm
	// lamda_red = 650 nm
	//	=> n_b = 1.5232
	//	   n_g = 1.5207
	//	   n_r = 1.5145
	float n_r = 1.5145;
	float n_g = 1.5207;
	float n_b = 1.5232;

	// water B = 1.3199
	//		 C = 0.006878 mum²
	//	=> n_b = 1.35038
	//	   n_g = 1.34634
	//	   n_r = 1.33617
	n_b = 1.35038;
	n_g = 1.34634;
	n_r = 1.33617;
	
	// Fused silica B = 1.4580
	//		 		C = 0.00354 mum²
	//	=> n_b = 1.47368
	//	   n_g = 1.47161
	//	   n_r = 1.46637
	//n_b = 1.47368;
	//n_g = 1.47161;
	//n_r = 1.46637;

	float rEta = 1.0/n_r;
	float gEta = 1.0/n_g;
	float bEta = 1.0/n_b;

	vec3 rRefractionDir = refract(I,N,rEta);
	vec3 gRefractionDir = refract(I,N,gEta);
	vec3 bRefractionDir = refract(I,N,bEta);

	vec4 rRefractionColor = texture(skybox,rRefractionDir);
	vec4 gRefractionColor = texture(skybox,gRefractionDir);
	vec4 bRefractionColor = texture(skybox,bRefractionDir);

	vec4 refractionColor = vec4(0.0);

	refractionColor.r = rRefractionColor.r;
	refractionColor.g = gRefractionColor.g;
	refractionColor.b = bRefractionColor.b;

	float uF_0 = 0.02;
	float fresnelTerm = F_Schlick(uF_0,V, N);
	color.rgb = vec3(fresnelTerm);


	color.rgb = mix(refractionColor.rgb, reflectionColor.rgb, fresnelTerm);


}