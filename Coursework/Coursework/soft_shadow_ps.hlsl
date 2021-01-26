
Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
SamplerState diffuseSampler  : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
	float4 ambient;
	float4 diffuse;
	float4 direction;
	float shadowBias;
	float3 padding;
	float4 diffuse2;
	float4 direction2;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : TEXCOORD1;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}


float4 main(InputType input) : SV_TARGET
{
	float bias;
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;

	float shadowMapBias = 0.019;
	// Set the default output color to be black (shadow).
	color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	// Calculate the projected texture coordinates.
	projectTexCoord.x = input.lightViewPos.x / input.lightViewPos.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPos.y / input.lightViewPos.w / 2.0f + 0.5f;

	//   // float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);
	//
	//	// Calculate the projected texture coordinates.
	//    float2 pTexCoord = input.lightViewPos.xy / input.lightViewPos.w;
	//	pTexCoord *= float2(0.5, -0.5);
	//	pTexCoord += float2(0.5f, 0.5f);
	//
	//    // Determine if the projected coordinates are in the 0 to 1 range.  If not don't do lighting.
	//    if (pTexCoord.x < 0.f || pTexCoord.x > 1.f || pTexCoord.y < 0.f || pTexCoord.y > 1.f){
	//        return textureColour;
	//    }
	//	
	//    // Sample the shadow map (get depth of geometry)
	//    depthValue = depthMapTexture.Sample(shadowSampler, pTexCoord).r;
	//
	//	// Calculate the depth from the light.
	//    lightDepthValue = input.lightViewPos.z / input.lightViewPos.w;
	//	lightDepthValue -= shadowMapBias;
	//
	//	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	//    if (lightDepthValue < depthValue){
	//        colour = calculateLighting(-direction.xyz, input.normal, diffuse);
	//    }
	//    return saturate(colour + ambient) * textureColour;
	//}

	// Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		depthValue = depthMapTexture.Sample(shadowSampler, projectTexCoord).r;

		// Calculate the depth of the light.
		lightDepthValue = input.lightViewPos.z / input.lightViewPos.w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - shadowMapBias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
		if (lightDepthValue < depthValue)
		{
			// Calculate the amount of light on this pixel.
			lightIntensity = saturate(dot(input.normal, input.position));

			// If this pixel is illuminated then set it to pure white (non-shadow).
			if (lightIntensity > 0.0f)
			{
				//And if we determine a pixel is illuminated then it is colored pure white.

				color = float4(1.0f, 1.0f, 1.0f, 1.0f);
			}
		}
	}
	color.w = 1;
	//This will return a black and white image of the shadowed scene so we can use it as input to the blurring shader.
	return color;
}