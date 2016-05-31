#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D G_PositionDepth;
uniform sampler2D G_Normal;
uniform sampler2D G_Color;

const int MAX_SAMPLE_SIZE = 16;
uniform vec3 samples[MAX_SAMPLE_SIZE];

uniform sampler2D noiseTexture;

uniform vec2 uNoiseScale; 

uniform mat4 projectionMatrix;

void main()
{ 

	// CALCULATE OCCLUSION FACTOR
	// 1°/ recover fragment's view space position/normal and depth
	// 2°/ we will then sample the depth buffer and count how many samples are occulding current fragment
	//	- reorient sample kernel along fragment normal: Change-Of-Basis Matrix
	//	- random rotation around normal to tilt the sample kernel

	// recover fragment position/depth/normal from GBuffer
	vec3 fragPos = texture(G_PositionDepth,TexCoords).rgb;
	float fragDepth = texture(G_PositionDepth,TexCoords).w;
	vec3 fragNormal = texture(G_Normal,TexCoords).rgb;

	vec3 rvec = 2.0 * texture(noiseTexture, TexCoords * uNoiseScale).xyz - 1.0; // random rotation vector

	// sample kernel position is in world space in the unit sphere (or hemisphere).
	// we need to position this kernel around fragment normal:
	// [0,1]^3 -> (N,T,BT) local space around the fragment (oriented in the direction of its normal).
	// incorporate rotation in change-of-basis matrix
	// => random tangent vector from normal and rvec
	vec3 fragTangent = normalize(rvec - fragNormal * dot(rvec, fragNormal)); 
	vec3 fragBitangent = cross(fragNormal, fragTangent);
	mat3 TBN = mat3(fragTangent, fragBitangent, fragNormal);

	float occlusion = 0.0;
	float uRadius = 1.0; // sampling radius

	// for each sample check if they occule fragment
	for (int i = 0; i < MAX_SAMPLE_SIZE; ++i) {
		// get sample position:
		// [0,1]^3 => fragment local space (0,0,0 = frag positon (world space), x = frag normal, y = frag tangent, z = frag bi-tangent)
		vec3 sample = TBN * samples[i];
		// scale sample position vector and get sample world space position 
		sample = sample * uRadius + fragPos;

		// project sample position in viewspace:
		vec4 offset = vec4(sample, 1.0);
		offset = projectionMatrix * offset;
		offset.xy /= offset.w;
		offset.xy = 0.5 * offset.xy + 0.5;

		// get sample depth:
		float sampleDepth = -texture(G_PositionDepth, offset.xy).w;

		// range check & accumulate:
		float rangeCheck = smoothstep(0.0,1.0, uRadius/abs(fragPos.z - sampleDepth));
		//rangeCheck *= rangeCheck;

		occlusion += (sampleDepth >= sample.z ? 1.0 : 0.0) * rangeCheck; 
	}
	occlusion = 1.0 - occlusion/float(MAX_SAMPLE_SIZE);

	color.rgb = vec3(occlusion);
	color.w = fragDepth;

}