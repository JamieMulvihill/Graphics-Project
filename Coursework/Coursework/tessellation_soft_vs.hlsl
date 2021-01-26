struct InputType
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

OutputType main(InputType input)
{
	OutputType output;

	// Pass the vertex position into the hull shader.
	output.position = input.position;

	// Pass the input color into the hull shader.
	// output.colour = float4(1.0, 0.0, 0.0, 1.0);
	output.normal = input.normal;
	output.tex = input.tex;


	return output;
}
