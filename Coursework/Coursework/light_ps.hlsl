Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer DirectionalLightBuffer : register(b0)
{
	float4 ambientColor;
	float4 directdiffuseColour;
	float4 lightDirection;
};

cbuffer LightBuffer : register(b1)
{
	float4 ambientColour;
	float4 diffuseColour;
	float4 position;
	float4 specularPower;
	float4 specularColour;
};

cbuffer SpotLightBuffer : register(b2)
{
	float4 ambient;
	float4 diffuse;
	float3 spotPosition;
	float angleRad;
	float3 direction;
	int lightSetting;
};

cbuffer AttenuationBuffer : register(b3) {
	float constant_Factor;
	float linear_Factor;
	float quadratic_Factor;
	float pad;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
	float3 viewVector: TEXCOORD2;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calcDirectionalLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);

	return colour;
}

float4 calcPointLighting(float3 lightDirection, float3 normal, float4 diffuse, float3 worldPosition, float4 position, float quadraticFactor, float linearFactor, float constantFactor) {

	float intensity = saturate(dot(normal, lightDirection));

	float4 colour = saturate(diffuse * intensity);

	float distance = length(position.xyz - worldPosition);
	float attenuation = 1 / (constantFactor + (linearFactor * distance) + (quadraticFactor * pow(distance, 2)));

	return colour * attenuation;
}

float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float4 specularPower) {
	// blinn-phong specular calculation
	float3 halfway = normalize(lightDirection + viewVector);
	float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);

	return saturate(specularColour * specularIntensity);
}

float4 main(InputType input) : SV_TARGET
{
	float4 textureColour;
	float4 lightColour, lightColor;

	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	// calculating the point light
	textureColour = texture0.Sample(sampler0, input.tex);
	float3 lightVector = normalize(position.xyz - input.worldPosition);
	lightColour = calcPointLighting(lightVector, input.normal, diffuseColour, input.worldPosition, position, quadratic_Factor, linear_Factor, constant_Factor);
	lightColour.w = 1;


	//calculating the spot light values
	float3 spotlightVector = normalize(spotPosition - input.worldPosition);
	float3 spotlightVecIn = -spotlightVector;

	float cosA = dot(direction, spotlightVecIn);
	float cosAngle = cos(angleRad);
	float4 spotlightColour;

	spotlightColour = calcPointLighting(spotlightVector, input.normal, diffuse, input.worldPosition, float4(spotPosition, 1), 0.f, 0.125f, 0.5f);
	float result = pow(max(cosA, cosAngle), 50);
	float4 finalColor;
	finalColor = (spotlightColour) * result;
	

	// setting the lighting values
	float4 outputColour;
	if (lightSetting == 2) {
		outputColour = saturate((lightColour + ambient) * textureColour);
		outputColour.w = 1;
	}
	else if (lightSetting == 3) {
		outputColour = saturate((finalColor + ambient) * textureColour);
		outputColour.w = 1;
	}
	else
		outputColour = ((lightColour + finalColor + ambient) * textureColour);
		outputColour.w = 1;


	return outputColour;
}

