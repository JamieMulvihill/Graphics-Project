Texture2D shaderTexture : register(t0);
//The shadowTexture is the black and white blurred image that contains the soft shadows.We will project this texture onto the scene.
Texture2D shadowTexture : register(t1);

SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);

cbuffer LightBuffer:  register(b0)
{
	float4 ambientColor;
	float4 diffuseColor;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 viewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

float4 main(InputType input) : SV_TARGET
{
	float4 color;
	float lightIntensity;
	float4 textureColor;
	float2 projectTexCoord;
	float shadowValue;

	//Calculate the lighting and texturing as normal.
	// Set the default output color to the ambient light value for all pixels.
	color = ambientColor;
	// Calculate the amount of light on this pixel.
	lightIntensity = saturate(dot(input.normal, input.lightPos));
	if (lightIntensity > 0.0f)
	{
		// Determine the light color based on the diffuse color and the amount of light intensity.
		color += (diffuseColor * lightIntensity);

		// Saturate the light color.
		color = saturate(color);
	}

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);

	// Combine the light and texture color.
	color = color * textureColor;
	//Now calculate the projected coordinates to sample the blurred black and white image from based on the camera's view point.

	//// Calculate the projected texture coordinates to be used with the shadow texture.
	//projectTexCoord.x = input.viewPosition.x / input.viewPosition.w / 2.0f + 0.5f;
	//projectTexCoord.y = -input.viewPosition.y / input.viewPosition.w / 2.0f + 0.5f;

	float2 pTexCoord = input.viewPosition.xy / input.viewPosition.w;
	pTexCoord *= float2(0.5, -0.5);
	pTexCoord += float2(0.5f, 0.5f);

	//Sample the soft shadow image using the projected coordinates and then multiple it in the same fashion as a light map to the final color to get the soft shadow effect.
	// Sample the shadow value from the shadow texture using the sampler at the projected texture coordinate location.
	shadowValue = shadowTexture.Sample(SampleTypeClamp, pTexCoord).r;

	// Combine the shadows with the final color.
	color = color * shadowValue;
	color.w = 1.f;
	return color;
};