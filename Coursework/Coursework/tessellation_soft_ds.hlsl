// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
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


cbuffer TessellationBuffer : register(b2)
{
	float4 tesselationFactor;
	float2 insideTessellation;
	float2 pad;
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
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 viewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition, normPosition;
	float2 texCoOrd;
	OutputType output;


	//patch[1].position = sin(patch[0].position + 1);
	// Determine the position of the new vertex.
	// Invert the y and Z components of uvwCoord as these coords are generated in UV space and therefore y is positive downward.
	// Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).//vertexPosition.z = sin((vertexPosition.y * 2) + (padding.x * 2)) * 2;
	//vertexPosition.z += cos((vertexPosition.x * 2) + (padding.x * 2)) * 2;


	float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
	vertexPosition = lerp(v1, v2, uvwCoord.x);

	float2 uv1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
	float2 uv2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
	texCoOrd = lerp(uv1, uv2, uvwCoord.x);

	float3 norm1 = lerp(patch[0].normal, patch[1].normal, uvwCoord.y);
	float3 norm2 = lerp(patch[3].normal, patch[2].normal, uvwCoord.y);
	normPosition = lerp(norm1, norm2, uvwCoord.x);

	//output.normal = normPosition;

	// Send the input color into the pixel shader.
	//output.tex = texCoOrd;
	float4 worldPosition;
	float4 ds_position = float4(vertexPosition, 1);

	float textureColor = texture0.SampleLevel(sampler0, texCoOrd, 0);
	float offest = (textureColor * 20);
	ds_position.y += 1 * offest;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(ds_position, worldMatrix);
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
	output.tex = texCoOrd;

	// Calculate the normal vector against the world matrix only.
	output.normal = mul(normPosition, (float3x3)worldMatrix);

	// Normalize the normal vector.
	output.normal = normalize(output.normal);

	// Calculate the position of the vertex in the world.
	worldPosition = mul(ds_position, worldMatrix);

	// Determine the light position based on the position of the light and the position of the vertex in the world.
	output.lightPos = lightPosition.xyz - worldPosition.xyz;

	// Normalize the light position vector.
	output.lightPos = normalize(output.lightPos);

	return output;
}

