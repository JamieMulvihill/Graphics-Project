// Basic shader forrendering textured geometry
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

cbuffer LightBuffer : register(b1)
{
	float3 lightPosition;
	float padding;
};

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 viewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

OutputType main(InputType input)
{
	OutputType output;

	float4 worldPosition;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	float textureColor = texture0.SampleLevel(sampler0, input.tex, 0);
	float offest = (textureColor * 20);
	input.position.xyz += input.normal * offest;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//The viewPosition will be used to calculate the projection coordinates to project the soft shadows onto the scene.
	// Store the position of the vertice as viewed by the camera in a separate variable.
	// Calculate the position of the vertice as viewed by the light source.
	/*output.viewPosition = mul(input.position, worldMatrix);
	output.viewPosition = mul(output.viewPosition, lightViewMatrix);
	output.viewPosition = mul(output.viewPosition, lightProjectionMatrix);*/

	// Store the position of the vertice as viewed by the camera in a separate variable.
	output.viewPosition = output.position;


	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only.
	output.normal = mul(input.normal, (float3x3)worldMatrix);

	// Normalize the normal vector.
	output.normal = normalize(output.normal);

	// Calculate the position of the vertex in the world.
	worldPosition = mul(input.position, worldMatrix);

	// Determine the light position based on the position of the light and the position of the vertex in the world.
	output.lightPos = lightPosition.xyz - worldPosition.xyz;

	// Normalize the light position vector.
	output.lightPos = normalize(output.lightPos);

	return output;
}