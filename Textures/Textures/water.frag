#version 330 core
in vec3 vNormal;
in vec2 TexCoord;
in vec3 vPosition;
in vec3 lDirection;

out vec4 color;

// Texture samplers
uniform sampler2D wallTexture;
uniform sampler2D waterDUDV;
uniform sampler2D waterNormal;

uniform float xOffset;

uniform samplerCube skybox;



void main()
{

	
    vec3 N = normalize(vNormal);
    vec3 V = normalize(vec3(0,0,3)-vPosition);	



	// reflection
	//	vec3 cameraPos = vec3(0.0,0.0,3.0);
	//	vec3 I = normalize(vPosition - cameraPos);
	//	vec3 reflection = reflect(I, N);

	// refraction
	//	float eta = 1.0/1.52;
	//	vec3 refraction = refract(I, N, eta);


	// water riples
	const float rippleSpeed = 0.2;


	// we distord TexCoord slightly by using a DUDV map and a random offset
	vec2 distortedTexCoord =  0.1 * texture(waterDUDV, vec2( TexCoord.x + rippleSpeed*xOffset,  TexCoord.y)).rg;
	distortedTexCoord = TexCoord + vec2(distortedTexCoord.x, distortedTexCoord.y + rippleSpeed*xOffset);

	// normalize back to 0,1
	vec2 totalDistortion = 2.0*texture(waterDUDV,distortedTexCoord).rg - 1.0;
	
	// water riples
	const float rippleWaveLength = 0.007;
	totalDistortion *= rippleWaveLength;



	// refraction => sample from wall texture at shifted TexCoords
	vec2 refractionTexCoord = TexCoord + totalDistortion;
	//refractionTexCoord.x = clamp(refractionTexCoord.x,0.001,0.999);

	vec4 refractionColor = texture(wallTexture, refractionTexCoord);


	// distorted water normal
	// => oriented in 0,0,1 direction
	vec2 reflectionTexCoord = TexCoord + totalDistortion;

	vec3 dN = texture(waterNormal, reflectionTexCoord).rgb;
	dN = 2.0 * dN - 1.0;
	// change of basis to local N frame & thus orientation


	
	vec3 Z = vec3(0,0,1);
	vec3 ZxN = cross(Z,N);
	float s = length(ZxN); // sin of angle
	float c = dot(Z,N); // cos of angle


	mat3 vX = mat3(vec3(0,-ZxN.z,ZxN.y),vec3(ZxN.z,0,-ZxN.x),vec3(-ZxN.y,ZxN.x,0));
	mat3 R_TBN = mat3(1.0) + vX + vX * vX * (1-c)/(s*s + 0.0001);


	dN = R_TBN * dN;
	dN = normalize(dN);


	vec3 I = -V;
	vec3 reflection = reflect(I, dN);

	vec4 reflectionColor = texture(skybox, reflection);


	// blend between relfection and refraction
	float refractivePower = clamp(dot(dN,V),0.0,1.0);
	refractivePower = pow(refractivePower,5.0);
	color = mix(reflectionColor, refractionColor,refractivePower);

}