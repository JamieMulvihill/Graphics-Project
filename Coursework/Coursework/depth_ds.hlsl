Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

};
cbuffer TessellationBuffer : register(b1)
{
	float4 cameraPosition;
};
struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD1;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition, normPosition;
	float2 texCoOrd;
	OutputType output;

	float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
	vertexPosition = lerp(v1, v2, uvwCoord.x);

	float2 uv1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
	float2 uv2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
	texCoOrd = lerp(uv1, uv2, uvwCoord.x);

	float3 norm1 = lerp(patch[0].normal, patch[1].normal, uvwCoord.y);
	float3 norm2 = lerp(patch[3].normal, patch[2].normal, uvwCoord.y);
	normPosition = lerp(norm1, norm2, uvwCoord.x);

	float textureColor = texture0.SampleLevel(sampler0, texCoOrd, 0).r;
	float offest = (textureColor * 20);
	vertexPosition += normPosition * offest;

	// Calculate the position of the new vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.depthPosition = output.position;

	return output;
}

