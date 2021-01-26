Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer DisplacementBuffer : register(b1) {
	float displacement;
	float3 padding;
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
	float4 depthPosition : TEXCOORD0;
};

OutputType main(InputType input)
{
	OutputType output;

	float textureColor = texture0.SampleLevel(sampler0, input.tex, 0).r;
	float offset = (textureColor * displacement);
	input.position.y += input.normal.y * offset;
	input.position.x += input.normal.x * offset;
	input.position.z += input.normal.z * offset;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the position value in a second input value for depth value calculations.
	output.depthPosition = output.position;

	return output;
}