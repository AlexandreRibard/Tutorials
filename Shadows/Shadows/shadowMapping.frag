#version 330 core
in vec3 vNormal;
in vec2 TexCoord;
in vec3 vPosition;
in vec4 vPositionLightSpace;

out vec4 color;

// Texture samplers
uniform sampler2D wallTexture;

uniform sampler2D depthMap;


const float PI = 3.141592653589793238462643383;

float LinearizeDepth(float depth) 
{
	float near = 1.0; 
	float far  = 100.0; 

    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near * far) / (z * (far - near) - (far + near));	
}

float linstep(float low, float high, float v){
    return clamp((v-low)/(high-low), 0.0, 1.0);
}

float reduceLightBleeding(float p_max, float Amount)
{
	return linstep(Amount,1.0,p_max);
}

float ChebyshevUpperBound(vec2 moments, float t)  
{  
	// One-tailed inequality valid if t > Moments.x  
	float p = float(t <= moments.x); 
	// Compute variance.  
	float variance = moments.y - (moments.x * moments.x);

	// Compute probabilistic upper bound.
	float d = t - moments.x;

	float p_max = variance / (variance + d*d);

	return max(p,p_max);
}  


float SampleVarianceShadowMap(vec2 coords, float compare)
{
	vec2 moments = texture(depthMap, coords.xy).xy;

	float p = step(compare,moments.x);
	float variance = max(moments.y - moments.x * moments.x,0.00002);


	float d = compare - moments.x;
	float pMax = variance / (variance + d*d);

	pMax = linstep(0.2,1.0,pMax);

	return min(max(p,pMax),1.0);

	//return step(compare,texture(depthMap, coords.xy).r);

}


void main()
{

	vec3 frag_Color = texture(wallTexture, TexCoord).rgb;
	color.rgb = frag_Color;

	vec3 projCoords = vPositionLightSpace.xyz;
	projCoords /= vPositionLightSpace.w; // -1,1
	projCoords = 0.5 * projCoords + 0.5; // 0,1

	float closestDepth = texture(depthMap, projCoords.xy).r;
	float currentDepth = 0.5*vPositionLightSpace.z + 0.5;

	float shadowPercentage = 0.0;
/*
	vec2 M1_M2 = texture(depthMap, projCoords.xy).rg;
	float mean = M1_M2.r;
	float variance = M1_M2.g - mean*mean;

	shadowPercentage = variance/(variance + (currentDepth - mean) * (currentDepth - mean));
	//shadowPercentage = linstep(0.2,1.0,shadowPercentage);

	if ( abs(currentDepth - mean) < bias)
	{
		color.rgb = vec3(1.0);
		shadowPercentage = 1.0;
	}
	else 
	{
		color.rgb = vec3(shadowPercentage);
	}

	color.rgb = shadowPercentage*frag_Color;
*/


/*
	shadowPercentage = ChebyshevUpperBound(M1_M2,currentDepth);
	shadowPercentage = reduceLightBleeding(shadowPercentage,0.2);

	color.rgb = shadowPercentage*frag_Color;
*/

	vec3 shadowMapCoords = vPositionLightSpace.xyz/vPositionLightSpace.w;
	shadowMapCoords = 0.5 * shadowMapCoords + 0.5; // 0,1

	shadowPercentage = SampleVarianceShadowMap(shadowMapCoords.xy,shadowMapCoords.z);
	color.rgb = shadowPercentage * frag_Color;
}