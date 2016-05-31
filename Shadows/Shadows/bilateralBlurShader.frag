#version 330 core

out vec4 color;


in vec2 TexCoords;


uniform sampler2D screenTexture;

const int blurSize = 2;
const float PI = 3.141592653589793238462643383;


float g_domaineWeighting(vec2 p, vec2 s)
{
	float sigma_d = 3.0;
	float d = length(p-s);

	return exp(-(d*d)/(2.0*sigma_d*sigma_d));

}


// fr: range kernel for smoothing differences in intensities
// Gaussian functions
float f_rangeWeighting(vec2 Ip, vec2 Is)
{
	float sigma_g = 3.0;
	float d = length(Ip-Is);


	return exp(-(d*d)/(2.0*sigma_g*sigma_g));

}
float f_rangeWeighting(vec3 Ip, vec3 Is)
{
	float sigma_g = 3.0;
	float d = length(Ip-Is);


	return exp(-(d*d)/(2.0*sigma_g*sigma_g));

}



void main()
{ 
	// GAUSSIAN BLUR:
	// g(x,y) = 1/2PIsigma2 * exp(-(x² + y²)/2sigma2)
	//		  = 1/2PIsigma2 *exp(-x²/2sigma2) * exp(-y²/2sigma2)
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
	
	// BI-LATERAL FILTER:
	// Ifiltered(x) = 1/Wp * Sum_{x_i in SamplingWindow} I(x_i) * fr(abs(I(x_i) - I(x))) * gs(abs(x_i - x))
	// Wp = Sum_{x_i in SamplingWindow} fr(abs(I(x_i) - I(x))) * gs(abs(x_i - x))
	//	=> fr: range kernel for smoothing differences in intensities
	//	   gs: spatial kernel for smoothing differences in coordiantes
	//
	// NB: we use gaussian functions for both fr & gs
	//
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
			float weight = f_rangeWeighting(Ip.rg,Is.rg) * g_domaineWeighting(p,vec2(0.0,0.0));


			Js += weight * Ip.rgb;
			Ks += weight;
        }
    }
	Js /= Ks;
	color.rgb = Js;
	



}