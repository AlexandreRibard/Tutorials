#version 330 core

out vec4 color;


in vec2 TexCoords;


uniform sampler2D screenTexture;

const int blurSize = 2;
const float PI = 3.141592653589793238462643383;


float f_domaineWeighting(vec2 p, vec2 s)
{
	float sigma_d = 3.0;
	float d = length(p-s);

	return exp(-(d*d)/(2.0*sigma_d*sigma_d));

}

float g_rangeWeighting(vec3 Ip, vec3 Is)
{
	float sigma_g = 3.0;
	float d = length(Ip-Is);


	return exp(-(d*d)/(2.0*sigma_g*sigma_g));

}

float g_rangeWeighting(vec4 Ip, vec4 Is)
{
	float sigma_g = 3.0;
	float d = length(Ip-Is);
	vec4 Idiff = abs(Ip - Is);
	d = ( (Idiff.r+Idiff.g+Idiff.b) + Idiff.w);



	return exp(-(d*d)/(2.0*sigma_g*sigma_g));

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

vec4 hejl(const in vec4 color)
{
	vec4 x = max(vec4(0.0), color - vec4(0.004));

	return (x * ((6.2 * x) + vec4(0.5))) / max(x * ((6.2 * x) + vec4(1.7)) + vec4(0.06), vec4(1e-8));
}


const mat3 YUV2RGB = mat3(vec3(1,1,1),vec3(0,-0.194208,1),vec3(1,-0.50937,0));

void main()
{ 
	/*
	float kernel[25] = float[](
		0.03765 ,	0.015019,	0.023792,	0.015019,	0.003765,
		0.015019,	0.059912,	0.094907,	0.059912,	0.015019,
		0.023792,	0.094907,	0.150342,	0.094907,	0.023792,
		0.015019,	0.059912,	0.094907,	0.059912,	0.015019,
		0.003765,	0.015019,	0.023792,	0.015019,	0.003765
    );


	vec2 texelSize = 1.0 / vec2(textureSize(screenTexture, 0));
    vec4 weigthedColor = vec4(0.0);

	vec4 pixelColor;

    for (int x = -blurSize; x <= blurSize; ++x) 
    {
        for (int y = -blurSize; y <= blurSize; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
			float kernelWeight = kernel[(x+2) + 5*(y+2)];

			pixelColor = texture(screenTexture, TexCoords + offset);

            weigthedColor += kernelWeight * pixelColor;
        }
    }
    color = weigthedColor;
	*/
	/*
	// bi-lateral filter
	// window [-blurSize x blurSize] x [-blurSize x blurSize]

	vec2 texelSize = 1.0 / vec2(textureSize(screenTexture, 0));
	vec4 Is = texture(screenTexture, TexCoords);
	vec3 Js = vec3(0.0,0.0,0.0);
	float Ks = 0.0;

	for (int x = -blurSize; x <= blurSize; ++x) 
    {
        for (int y = -blurSize; y <= blurSize; ++y) 
        {
			vec2 p =  vec2(float(x), float(y));
            vec2 offset = vec2(float(x), float(y)) * texelSize;
			vec4 Ip = texture(screenTexture, TexCoords + offset);

			// color filter
			//float weight = f_domaineWeighting(p,vec2(0.0,0.0)) * g_rangeWeighting(Ip.rgb,Is.rgb);
			// depth filter
			float weight = f_domaineWeighting(p,vec2(0.0,0.0)) * g_rangeWeighting(vec3(Ip.w),vec3(Is.w));
			// depth + color
			// float weight = f_domaineWeighting(p,vec2(0.0,0.0)) * g_rangeWeighting(Ip,Is);


			Js += weight * Ip.rgb;
			Ks += weight;
        }
    }
	Js /= Ks;
	color.rgb = Js;

	vec4 fragColor = vec4(Js.rgb,1.0);
	*/

	
	// sepia filter
	
	vec4 fragColor = texture2D( screenTexture, TexCoords );
	
	float luma = dot(fragColor.rgb, vec3(0.299, 0.587, 0.114));
	const vec3 SEPIA = vec3(1.2, 1.0, 0.8); 

	vec4 sepiaColor = vec4(vec3(luma) * SEPIA, 1.0);
	color = mix(fragColor,sepiaColor,0.5);
	
	color = fragColor;

	//color = pow(color, vec4(1.0/2.2));


}