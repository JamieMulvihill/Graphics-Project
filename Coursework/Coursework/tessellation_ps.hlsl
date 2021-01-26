Texture2D texture0 : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D depthMapTexture2 : register(t2);

SamplerState Sampler0 : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer DirectionalLightBuffer : register(b0) {

	float4 ambient;
	float4 diffuse;
	float4 direction;
	float shadowBias;
	int lightSetting;
	float2 padding;
};

cbuffer PointLightBuffer : register(b1)
{
	float4 ambientColor;
	float4 pointdiffuseColour;
	float4 position;
	float specularPower;
	float3 pads;
	float4 specularColor;
};

cbuffer SpotLightBuffer : register(b2)
{
	float4 ambientSpot;
	float4 diffuseSpot;
	float4 spotPosition;
	float angleRad;
	float3 directionSpot;
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
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	float4 lightViewPos : TEXCOORD1;
	float3 worldPos : TEXCOORD2;
	float4 lightViewPos2 : TEXCOORD3;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse) {

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

float4 main(InputType input) : SV_TARGET{

	float depthValue, depthValue2;
	float lightDepthValue, spotLightDepthValue;

	float shadowMapBias = shadowBias;
	float4 shadowColour = float4(0.f, 0.f, 0.f, 1.f);
	float4 spotlightColour = float4(0.f, 0.f, 0.f, 1.f);
	float4 finalColor;
	float4 spotLightShadowColour = float4(0.f, 0.f, 0.f, 1.f);
	float4 textureColour = texture0.Sample(Sampler0, input.tex);

	// Calculate the projected texture coordinates.
	float2 pTexCoord = input.lightViewPos.xy / input.lightViewPos.w;
	pTexCoord *= float2(0.5, -0.5);
	pTexCoord += float2(0.5f, 0.5f);

	// Determine if the projected coordinates are in the 0 to 1 range.  If not don't do lighting.
	if (pTexCoord.x < 0.f || pTexCoord.x > 1.f || pTexCoord.y < 0.f || pTexCoord.y > 1.f){
		return textureColour;
	}

	// Calculate the projected texture coordinates.
	float2 pTexCoord2 = input.lightViewPos2.xy / input.lightViewPos2.w;
	pTexCoord2 *= float2(0.5, -0.5);
	pTexCoord2 += float2(0.5f, 0.5f);
	if (pTexCoord2.x < 0.f || pTexCoord2.x > 1.f || pTexCoord2.y < 0.f || pTexCoord2.y > 1.f){

		return textureColour;
	}


	// Sample the shadow map (get depth of geometry)
	depthValue = depthMapTexture.Sample(shadowSampler, pTexCoord).r;
	
	// Calculate the depth from the light.
	lightDepthValue = input.lightViewPos.z / input.lightViewPos.w;
	lightDepthValue -= shadowMapBias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	// cvalculating the directional light
	if (lightDepthValue < depthValue)
	{
		shadowColour = calculateLighting(-direction.xyz, input.normal, diffuse);
	}



	// calcuating the spotlight and its shadows
	float3 spotlightVector = normalize(spotPosition.xyz - input.worldPos);
	float3 spotlightVecIn = -spotlightVector;
	float cosA = dot(directionSpot, spotlightVecIn);
	float cosAngle = cos(angleRad);
	float result = pow(max(cosA, cosAngle), 20);
	// Sample the shadow map (get depth of geometry)
	depthValue2 = depthMapTexture2.Sample(shadowSampler, pTexCoord2).r;
	spotLightDepthValue = input.lightViewPos2.z / input.lightViewPos2.w;
	spotLightDepthValue -= shadowMapBias;

	if (spotLightDepthValue < depthValue2)
	{
		spotlightColour = calcPointLighting(spotlightVector, input.normal, diffuseSpot, input.worldPos, spotPosition, 0.f, 0.125f, 0.5f);
	}
	finalColor = (spotlightColour)* result;



	// calculating the point light
	float3 pointLightVector;
	float4 pointLightColour;
	pointLightVector = normalize(position.xyz - input.worldPos);
	pointLightColour = calcPointLighting(pointLightVector, input.normal, pointdiffuseColour, input.worldPos, position, quadratic_Factor, linear_Factor, constant_Factor);

	//statments to change the lighting of the scene
	// 1 is just directional light, 2 is just point light, 3 is just spot light otherwise all the lights combined. 
	if (lightSetting == 1) {
		return saturate((shadowColour + ambient) * textureColour);
	}
	else if (lightSetting == 2) {
		return saturate((pointLightColour + ambient) * textureColour);
	}
	else if (lightSetting == 3) {
		return saturate((finalColor + ambient) * textureColour);
	}
	else 
		return saturate((finalColor + pointLightColour + shadowColour + ambient) * textureColour);
}
