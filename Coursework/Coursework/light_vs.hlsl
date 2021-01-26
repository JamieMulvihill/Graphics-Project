// Light vertex shader
// Standard issue vertex shader, apply matrices, pass info to pixel shader
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer: register(b1)
{
	float3 cameraPosition;
	float padding;
};

cbuffer DisplacementBuffer : register(b2) {
	float displacement;
	float3 padding2;
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
	float3 worldPos : TEXCOORD1;
	float3 viewVector: TEXCOORD2;
};

OutputType main(InputType input)
{
	OutputType output;

	float textureColor = texture0.SampleLevel(sampler0, input.tex, 0).r;
	float offest = (textureColor * displacement);
	input.position.y += input.normal.y * offest;
	input.position.x += input.normal.x * offest;
	input.position.z += input.normal.z * offest;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// Calculate the position of the vertex in the world.
	float3 worldPosition = mul(input.position, worldMatrix);
	output.worldPos = worldPosition;
	output.viewVector = cameraPosition.xyz - worldPosition.xyz;
	output.viewVector = normalize(output.viewVector);

	return output;
}