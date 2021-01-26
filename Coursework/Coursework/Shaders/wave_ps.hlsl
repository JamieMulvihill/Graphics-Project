
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
	float4 ambient;
	float4 diffuse;
	float4 position;
	float4 specularColour;
	float4 specularPower;
};
cbuffer AttenuationBuffer : register(b1) {
	float constantFactor;
	float linearFactor;
	float quadraticFactor;
};
struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
	float3 viewVector: TEXCOORD2;
};

float4 calcPointLighting(float3 lightDirection, float3 normal, float4 diffuse, float3 worldPosition, float4 position) {

	float intensity = saturate(dot(normal, lightDirection));

	float4 colour = saturate(diffuse * intensity);

	float distance = length(position.xyz - worldPosition);
	float attenuation = 1 / (constantFactor + (linearFactor * distance) + (quadraticFactor * pow(distance, 2)));

	return colour;// *attenuation;
}


float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float specularPower) {

	// blinn-phongspecular calculation
	float3 halfway = normalize(lightDirection + viewVector);
	float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);

	return saturate(specularColour * specularIntensity);
}

float4 main(InputType input) : SV_TARGET
{
	float4 textureColour;
	float4 lightColour;

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	textureColour = texture0.Sample(sampler0, input.tex);
	float3 lightVector = normalize(position.xyz - input.worldPosition);
	lightColour = calcPointLighting(lightVector, input.normal, diffuse, input.worldPosition, position);
	
	textureColour = (lightColour * textureColour);
	textureColour.w = .5f;
	return textureColour + calcSpecular(lightVector, input.normal, input.viewVector, specularColour, specularPower.x);
}
