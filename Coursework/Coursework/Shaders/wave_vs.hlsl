// logic for the wave vertex manipulation adapted from https://catlikecoding.com/unity/tutorials/flow/waves/

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer TimeBuffer : register(b1)
{
	float time;
	float amplitude;
	float frequency;
	float speed;
	float4 DirectionalVector;
};
cbuffer CameraBuffer: register(b2)
{
	float3 cameraPosition;
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
	float3 worldPosition : TEXCOORD1;
	float3 viewVector: TEXCOORD2;
};
float3 GerstnerWave(float frequency_, float2 DirectionalVector_, float amplitude_, float3 position, inout float3 tangent, inout float3 binormal) {

	// Grestner Wave Calculations
	float wavelength = 2 * 3.15159 / frequency_;
	// Gravity is used to calculate a realistic wave speed
	float wave_speed = sqrt(9.8 / wavelength);
	float2 direction = normalize(DirectionalVector_);
	// this limiter value is used to stop the waves rounding on itself when the angles get too high 
	float limiter = amplitude_ / wavelength;
	// deltawave is the change calculated by the manipulation of the vertex
	float delta_wave = wavelength * (dot(direction, position.xz) - wave_speed * time);

	// calclating the normals for the x axis by creating a tangent vector, as vertex manipulation is also carried out along the
	// z axis, the normals must also be calculated using a binormal tangent vector. The cross product of these vectors
	//could then be used for the normals of the wave
	tangent += float3(
		1 - direction.x * direction.x * (amplitude_ * sin(delta_wave)),
		direction.x * (amplitude_ * cos(delta_wave)),
		-direction.x * direction.y * (amplitude_ * sin(delta_wave))
		);

	binormal += float3(
		-direction.x * direction.y * (amplitude_ * sin(delta_wave)),
		direction.y * (amplitude_ * cos(delta_wave)),
		1 - direction.y * direction.y * (amplitude_ * sin(delta_wave))
		);

	// manipulation of each vertex by the values of the wave calculation
	position.x += direction.x * (limiter * cos(delta_wave));
	position.y = limiter * sin(delta_wave);
	position.z += direction.y * (limiter * cos(delta_wave));

	return position;

}
OutputType main(InputType input)
{
	OutputType output;

	float3 tangent = float3(0, 0, 0);
	float3 binormal = float3(0, 0, 0);

	float3 pointP = input.position.xyz;
	float3 wavePosition = pointP;

	//calculating the wave value
	wavePosition = GerstnerWave(frequency, DirectionalVector, amplitude, pointP, tangent, binormal);

	input.normal = normalize(cross(binormal, tangent));
	input.position.xyz = wavePosition;
	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	output.worldPosition = mul(input.position, worldMatrix).xyz;
	output.viewVector = cameraPosition.xyz - output.worldPosition.xyz;
	output.viewVector = normalize(output.viewVector);


	return output;
}