// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
// logic for the sobel calculations adapted from the book "Practical Rendering"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
	matrix lightViewMatrix2;
	matrix lightProjectionMatrix2;
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
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	float4 lightViewPos : TEXCOORD1;
	float3 worldPos : TEXCOORD2;
	float4 lightViewPos2 : TEXCOORD3;
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
	output.normal = normPosition;

	// Send the input color into the pixel shader.
	output.tex = texCoOrd;

	float textureColor = texture0.SampleLevel(sampler0, output.tex , 0).r;
	float offest = (textureColor * 20);
	vertexPosition += normPosition * offest; 

	float2 pixelSize = float2(1.f / 1024, 1.f / 1024);

	// computing pixel offset values
	float2 topLeft = output.tex + float2(-pixelSize.x, -pixelSize.y);
	float2 topCentre = output.tex + float2(0, -pixelSize.y);
	float2 topRight = output.tex + float2(pixelSize.x, -pixelSize.y);

	float2 centreLeft = output.tex + float2(-pixelSize.x, 0.f);
	float2 centreRight = output.tex + float2(pixelSize.x, 0.f);

	float2 bottomLeft = output.tex + float2(-pixelSize.x, pixelSize.y);
	float2 bottomCentre = output.tex + float2(0, pixelSize.y);
	float2 bottomRight = output.tex + float2(pixelSize.x, pixelSize.y);

	float scale = 3;

	// apply sobel filter to surrounding current pixel
	float resTopLeft = scale * texture0.SampleLevel(sampler0, topLeft, 0).r;
	float resTopCentre = scale * texture0.SampleLevel(sampler0, topCentre, 0).r;
	float resTopRight = scale * texture0.SampleLevel(sampler0, topRight, 0).r;

	float resCentreLeft = scale * texture0.SampleLevel(sampler0, centreLeft, 0).r;
	float resCentreRight = scale * texture0.SampleLevel(sampler0, centreRight, 0).r;

	float resBottomLeft = scale * texture0.SampleLevel(sampler0, bottomLeft, 0).r;
	float resBottomCentre = scale * texture0.SampleLevel(sampler0, bottomCentre, 0).r;
	float resBottomRight = scale * texture0.SampleLevel(sampler0, bottomRight, 0).r;

	//execute sobel filters
	float Gx = resTopLeft - resTopRight + 2.f * resCentreLeft - 2.f * resCentreRight + resBottomLeft - resBottomRight;
	float Gy = resTopLeft + 2.f * resTopCentre + resTopRight - resBottomLeft - 2.f * resBottomCentre - resBottomRight;
	
	//Generate the so far missing Z values
	float Gz = 0.5f * sqrt(max(0.f, 1.f - Gx * Gx - Gy * Gy));

	output.normal = normalize(float3(2.f * Gx, Gz, 2.f * Gy));
	output.normal = mul(output.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPos = mul(float4(vertexPosition, 1), worldMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);

	float3 worldPosition = mul(vertexPosition, worldMatrix);
	output.worldPos = worldPosition;

	//light postion of the second light
	output.lightViewPos2 = mul(float4(vertexPosition, 1), worldMatrix);
	output.lightViewPos2 = mul(output.lightViewPos2, lightViewMatrix2);
	output.lightViewPos2 = mul(output.lightViewPos2, lightProjectionMatrix2);

	return output;
}

