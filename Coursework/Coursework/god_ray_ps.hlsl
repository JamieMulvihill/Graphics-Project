// logic for God Ray post process effect adapted from https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch13.html 
// and https://medium.com/community-play-3d/god-rays-whats-that-5a67f26aeac2

SamplerState sceneSampler : register(s0);
SamplerState blackSampler : register(s1);
Texture2D blackTexture : register(t0);
Texture2D sceneTexture : register(t1);

cbuffer GodRayBuffer : register(b0)
{
	float4 lightPosition;
	float2 screeSize;
	float2 padding;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer GodRayBuffer : register(b1)
{
	float decay;
	float exposure;
	float density;
	float weight;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET{

	float4 lightPos = lightPosition;

	// convert the light postion to projection space
	lightPos = mul(lightPos, viewMatrix);
	lightPos = mul(lightPos, projectionMatrix);

	//conversion to clip and then screen space
	lightPos.xyz /= lightPos.w;
	lightPos.xy *= float2(0.5f, -0.5f);
	lightPos.xy += 0.5f;

	// The numbar of times the texture will be processed and edited
	int processes = 100;

	float2 texCoOrd = input.tex;
	float2 rayVector = (texCoOrd - lightPos.xy);
	rayVector *= 1.0 / float(processes) * density;

	// initial value of the ray strength
	float illuminationDecay = 1.0;

	//sampling the black pass texture at the input texture coordinat
	float4 textureColour = blackTexture.Sample(blackSampler, texCoOrd) * 0.4f;
	
	 for (int i = 0; i < processes; i++) {
		// moving texture coordinate by the distance to the light, sampling the black pass texture at this pixel,
		// editing the colour value based on the illumaination, addind the too texute colours to create change in image,
		// decrease the illumaination value by the rate of decay so that on the next ireation of loop as the next pixel is futher awy
		// the illumination value is lower, this loop creates the ray effect
		texCoOrd -= rayVector;
		float4 rayColour = blackTexture.Sample(blackSampler, texCoOrd) * 0.4f;
		rayColour *= illuminationDecay * weight;
		textureColour += rayColour;
		illuminationDecay *= decay;

	 }

	 //sampling the first pass scene texture
	 float4 realColor = sceneTexture.Sample(sceneSampler, input.tex);
	 // increase the brightness of the image to make it appear more lit from the rays
	 realColor *= 1.1f;

	 float4 finalRayEffect = float4(float3(textureColour.r, textureColour.g, textureColour.b) * exposure, 1);
	
	 //added the two colours together for end result
	 return float4(finalRayEffect + realColor);
}