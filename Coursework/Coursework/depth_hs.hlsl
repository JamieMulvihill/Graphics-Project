cbuffer TessellationBuffer : register(b0)
{
	float4 cameraPosition;
};
struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	float maxTess = 64;

	float3 mid_point_A = lerp(inputPatch[0].position, inputPatch[1].position, .5);
	float3 mid_point_B = lerp(inputPatch[1].position, inputPatch[2].position, .5);
	float3 mid_point_C = lerp(inputPatch[2].position, inputPatch[3].position, .5);
	float3 mid_point_D = lerp(inputPatch[3].position, inputPatch[0].position, .5);

	float distanceA = sqrt(pow(cameraPosition.x - mid_point_A.x, 2) + pow(cameraPosition.y - mid_point_A.y, 2) + pow(cameraPosition.z - mid_point_A.z, 2));
	float distanceB = sqrt(pow(cameraPosition.x - mid_point_B.x, 2) + pow(cameraPosition.y - mid_point_B.y, 2) + pow(cameraPosition.z - mid_point_B.z, 2));
	float distanceC = sqrt(pow(cameraPosition.x - mid_point_C.x, 2) + pow(cameraPosition.y - mid_point_C.y, 2) + pow(cameraPosition.z - mid_point_C.z, 2));
	float distanceD = sqrt(pow(cameraPosition.x - mid_point_D.x, 2) + pow(cameraPosition.y - mid_point_D.y, 2) + pow(cameraPosition.z - mid_point_D.z, 2));

	float3 mid_point = (inputPatch[0].position + inputPatch[1].position + inputPatch[2].position + inputPatch[3].position) / 4;
	float distance = sqrt(pow(cameraPosition.x - mid_point.x, 2) + pow(cameraPosition.y - mid_point.y, 2) + pow(cameraPosition.z - mid_point.z, 2));

	float tessFact = max((maxTess - distance), 1);
	float tessFactA = max(maxTess - distanceA, 1);
	float tessFactB = max(maxTess - distanceB, 1);
	float tessFactC = max(maxTess - distanceC, 1);
	float tessFactD = max(maxTess - distanceD, 1);

	// Set the tessellation factors for the three edges of the triangle.
	output.edges[0] = tessFactA;
	output.edges[1] = tessFactD;
	output.edges[2] = tessFactC;
	output.edges[3] = tessFactB;

	// Set the tessellation factor for tessallating inside the triangle.
	output.inside[0] = tessFact;
	output.inside[1] = tessFact;

	return output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	OutputType output;

	// Set the position for this control point as the output position.
	output.position = patch[pointId].position;
	output.normal = patch[pointId].normal;
	output.tex = patch[pointId].tex;

	return output;
}